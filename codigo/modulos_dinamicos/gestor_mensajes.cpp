#include "gestor_mensajes.h"

#include <istream>
#include <ostream>
#include <limits>
#include <stdexcept>

Fecha::Fecha(int dia, int mes, int anio) : valor_(0) {
    if (anio < 1 || anio > 9999 || mes < 1 || mes > 12)
        throw std::invalid_argument("Fecha invalida");
    const int dias[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int limite = dias[mes - 1];
    if (mes == 2 && anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0)) ++limite;
    if (dia < 1 || dia > limite) throw std::invalid_argument("Fecha invalida");
    valor_ = anio * 10000 + mes * 100 + dia;
}
int Fecha::getDia() const { return valor_ % 100; }
int Fecha::getMes() const { return valor_ / 100 % 100; }
int Fecha::getAnio() const { return valor_ / 10000; }
bool Fecha::operator==(const Fecha& otra) const { return valor_ == otra.valor_; }
bool Fecha::operator<=(const Fecha& otra) const { return valor_ <= otra.valor_; }

Mensaje::Mensaje(int id, int emisor, int receptor, const Fecha& fecha,
    const char* texto, std::size_t bytes, bool leido)
    : id_(id), emisor_(emisor), receptor_(receptor), fecha_(fecha),
      texto_(new char[bytes + 1]), bytes_(bytes), leido_(leido) {
    for (std::size_t i = 0; i <= bytes; ++i) texto_[i] = texto[i];
}
Mensaje::Mensaje(const Mensaje& otro)
    : Mensaje(otro.id_, otro.emisor_, otro.receptor_, otro.fecha_,
              otro.texto_, otro.bytes_, otro.leido_) {}
Mensaje::~Mensaje() { delete[] texto_; }
int Mensaje::getId() const { return id_; }
int Mensaje::getEmisor() const { return emisor_; }
int Mensaje::getReceptor() const { return receptor_; }
const Fecha& Mensaje::getFecha() const { return fecha_; }
const char* Mensaje::getTexto() const { return texto_; }
bool Mensaje::estaLeido() const { return leido_; }

GestorMensajes::GestorMensajes() : mensajes_(nullptr), cantidad_(0), capacidad_(0),
    ultimoId_(0), bytesMensajes_(0), iteraciones_(0) {}
GestorMensajes::~GestorMensajes() {
    for (int i = 0; i < cantidad_; ++i) delete mensajes_[i];
    delete[] mensajes_;
}
bool GestorMensajes::validarTexto(const char* texto, std::size_t& bytes) const {
    bytes = 0;
    if (!texto) return false;
    int caracteres = 0;
    bool contenido = false;
    // UTF-8: una letra acentuada cuenta como un caracter, no como dos bytes.
    while (texto[bytes]) {
        ++iteraciones_;
        unsigned char inicial = static_cast<unsigned char>(texto[bytes]);
        int largo = inicial < 0x80 ? 1 : inicial >= 0xC2 && inicial <= 0xDF ? 2 :
            inicial >= 0xE0 && inicial <= 0xEF ? 3 : inicial >= 0xF0 && inicial <= 0xF4 ? 4 : 0;
        if (!largo || ++caracteres > 512) return false;
        for (int i = 1; i < largo; ++i) {
            ++iteraciones_;
            unsigned char c = static_cast<unsigned char>(texto[bytes + i]);
            if (c < 0x80 || c > 0xBF) return false;
            if (i == 1 && ((inicial == 0xE0 && c < 0xA0) || (inicial == 0xED && c > 0x9F) ||
                (inicial == 0xF0 && c < 0x90) || (inicial == 0xF4 && c > 0x8F))) return false;
        }
        if (inicial != ' ' && inicial != '\t' && inicial != '\r' && inicial != '\n') contenido = true;
        bytes += largo;
    }
    return contenido;
}
void GestorMensajes::insertar(Mensaje* mensaje) {
    // Crecimiento geometrico: reserva al necesitarlo y solo mueve punteros.
    if (cantidad_ == capacidad_) {
        if (capacidad_ > std::numeric_limits<int>::max() / 2)
            throw std::length_error("Demasiados mensajes");
        int nueva = capacidad_ ? capacidad_ * 2 : 8;
        Mensaje** arreglo = new Mensaje*[nueva];
        for (int i = 0; i < cantidad_; ++i) { ++iteraciones_; arreglo[i] = mensajes_[i]; }
        delete[] mensajes_;
        mensajes_ = arreglo; capacidad_ = nueva;
    }
    int posicion = cantidad_;
    // Insercion cronologica; ID desempata mensajes del mismo dia.
    while (posicion > 0) {
        ++iteraciones_;
        const Mensaje& anterior = *mensajes_[posicion - 1];
        if (anterior.fecha_ <= mensaje->fecha_ &&
            (!(anterior.fecha_ == mensaje->fecha_) || anterior.id_ < mensaje->id_)) break;
        mensajes_[posicion] = mensajes_[posicion - 1];
        --posicion;
    }
    mensajes_[posicion] = mensaje;
    ++cantidad_;
    bytesMensajes_ += sizeof(Mensaje) + mensaje->bytes_ + 1;
    iteraciones_ += mensaje->bytes_ + 1; // Copia del texto realizada por el constructor.
    if (mensaje->id_ > ultimoId_) ultimoId_ = mensaje->id_;
}
bool GestorMensajes::cargarMensaje(const AdminAmistades& red, int id, int emisor, int receptor,
    const Fecha& fecha, const char* texto, bool leido) {
    std::size_t bytes = 0;
    if (id <= 0 || emisor == receptor || red.buscarUsuario(emisor) < 0 ||
        red.buscarUsuario(receptor) < 0 || !validarTexto(texto, bytes)) return false;
    for (int i = 0; i < cantidad_; ++i) {
        ++iteraciones_;
        if (mensajes_[i]->id_ == id) return false;
    }
    Mensaje* nuevo = new Mensaje(id, emisor, receptor, fecha, texto, bytes, leido);
    try { insertar(nuevo); }
    catch (...) { delete nuevo; throw; }
    return true;
}
bool GestorMensajes::enviarMensaje(const AdminAmistades& red, int emisor,
    const char* destinatario, const char* texto, const Fecha& fecha) {
    int posicion = red.buscarUsuario(destinatario);
    if (posicion < 0 || ultimoId_ == std::numeric_limits<int>::max()) return false;
    int receptor = red.getIdEnPosicion(posicion);
    if (!red.sonAmigos(emisor, receptor)) return false;
    return cargarMensaje(red, ultimoId_ + 1, emisor, receptor, fecha, texto, false);
}
void GestorMensajes::imprimir(const AdminAmistades& red, const Mensaje& m, std::ostream& salida) const {
    const char* nombre = red.getNombre(m.emisor_);
    salida << m.fecha_.getDia() << '/' << m.fecha_.getMes() << '/' << m.fecha_.getAnio()
           << " | " << (nombre ? nombre : "Usuario desconocido") << ": " << m.texto_ << '\n';
}
int GestorMensajes::leerNuevos(const AdminAmistades& red, int receptor, std::ostream& salida) {
    if (red.buscarUsuario(receptor) < 0) return -1;
    int total = 0;
    for (int i = 0; i < cantidad_; ++i) {
        ++iteraciones_;
        if (mensajes_[i]->receptor_ == receptor && !mensajes_[i]->leido_) {
            imprimir(red, *mensajes_[i], salida); ++total;
        }
    }
    if (!total) salida << "No hay mensajes nuevos.\n";
    if (!salida) return -1; // No marcar si no pudieron mostrarse todos.
    for (int i = 0; i < cantidad_; ++i) {
        ++iteraciones_;
        if (mensajes_[i]->receptor_ == receptor) mensajes_[i]->leido_ = true;
    }
    return total;
}
int GestorMensajes::buscarPorRangoDeFechas(const AdminAmistades& red, int receptor,
    const Fecha& inicio, const Fecha& fin, std::ostream& salida) const {
    if (!(inicio <= fin) || red.buscarUsuario(receptor) < 0) return -1;
    // Busqueda binaria del inicio: evita recorrer fechas anteriores al rango.
    int bajo = 0, alto = cantidad_;
    while (bajo < alto) {
        ++iteraciones_;
        int medio = bajo + (alto - bajo) / 2;
        if (inicio <= mensajes_[medio]->fecha_) alto = medio;
        else bajo = medio + 1;
    }
    int total = 0;
    for (int i = bajo; i < cantidad_ && mensajes_[i]->fecha_ <= fin; ++i) {
        ++iteraciones_;
        if (mensajes_[i]->receptor_ == receptor) { imprimir(red, *mensajes_[i], salida); ++total; }
    }
    if (!total) salida << "No hay mensajes en ese rango.\n";
    return salida ? total : -1;
}

namespace {
bool leerNumero(std::istream& entrada, int& numero) {
    int leido = 0;
    bool ok = static_cast<bool>(entrada >> leido);
    if (ok) numero = leido;
    if (!ok && !entrada.eof()) entrada.clear();
    entrada.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return ok;
}
bool leerLinea(std::istream& entrada, char* texto, int capacidad) {
    if (entrada.getline(texto, capacidad)) return true;
    if (!entrada.eof()) {
        entrada.clear(); entrada.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return false;
}
bool leerFecha(std::istream& entrada, std::ostream& salida, Fecha& fecha) {
    int dia, mes, anio;
    salida << "Dia mes anio (ejemplo 16 10 2026): ";
    if (!(entrada >> dia >> mes >> anio)) {
        if (!entrada.eof()) entrada.clear();
        entrada.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return false;
    }
    entrada.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    try { fecha = Fecha(dia, mes, anio); return true; }
    catch (const std::invalid_argument&) { return false; }
}
}
void GestorMensajes::mostrarMenu(const AdminAmistades& red, int usuario, const Fecha& fechaActual,
    std::istream& entrada, std::ostream& salida) {
    if (red.buscarUsuario(usuario) < 0) { salida << "Usuario inexistente.\n"; return; }
    int opcion = -1;
    while (opcion != 0 && entrada && salida) {
        ++iteraciones_;
        salida << "\nMENSAJES\n1. Enviar\n2. Leer nuevos\n3. Buscar por fechas\n0. Volver\nOpcion: ";
        if (!leerNumero(entrada, opcion)) { salida << "Ingresa un numero.\n"; continue; }
        if (opcion == 1) {
            char nombre[1025], texto[2049]; // 512 caracteres UTF-8, maximo 4 bytes cada uno.
            salida << "Nombre del amigo: ";
            if (!leerLinea(entrada, nombre, sizeof nombre)) { salida << "Nombre invalido.\n"; continue; }
            salida << "Texto (hasta 512 caracteres): ";
            if (!leerLinea(entrada, texto, sizeof texto)) { salida << "Texto demasiado largo.\n"; continue; }
            salida << (enviarMensaje(red, usuario, nombre, texto, fechaActual) ? "Mensaje enviado.\n" :
                "No se pudo enviar: verifica amistad y texto.\n");
        } else if (opcion == 2) leerNuevos(red, usuario, salida);
        else if (opcion == 3) {
            Fecha inicio, fin;
            salida << "Fecha inicial. ";
            if (!leerFecha(entrada, salida, inicio)) { salida << "Fecha invalida.\n"; continue; }
            salida << "Fecha final. ";
            if (!leerFecha(entrada, salida, fin)) { salida << "Fecha invalida.\n"; continue; }
            if (buscarPorRangoDeFechas(red, usuario, inicio, fin, salida) < 0)
                salida << "Rango invalido o error de salida.\n";
        } else if (opcion != 0) salida << "Opcion invalida.\n";
    }
}
int GestorMensajes::getCantidad() const { return cantidad_; }
const Mensaje* GestorMensajes::getMensaje(int posicion) const {
    return posicion >= 0 && posicion < cantidad_ ? mensajes_[posicion] : nullptr;
}
void GestorMensajes::reiniciarIteraciones() const { iteraciones_ = 0; }
unsigned long long GestorMensajes::getIteraciones() const { return iteraciones_; }
std::size_t GestorMensajes::getMemoriaReservada() const {
    return sizeof(*this) + static_cast<std::size_t>(capacidad_) * sizeof(Mensaje*) + bytesMensajes_;
}
