#include "admin_amistades.h"
#include "../core/monitor_recursos.h"
#include <istream>
#include <ostream>
#include <limits>
#include <stdexcept>

namespace {
bool leerEntero(std::istream &entrada, int &valor) {
    int leido = 0;
    if (entrada >> leido) {
        valor = leido;
        entrada.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return true;
    }
    if (!entrada.eof()) {
        entrada.clear();
        entrada.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return false;
}
} // namespace

AdminAmistades::AdminAmistades(int capacidad)
    : cambiosPendientes_(false), capacidad_(capacidad), cantidad_(0), ids_(nullptr),
      nombres_(nullptr), matriz_(nullptr), bytesNombres_(0) {
    if (capacidad <= 0 ||
        static_cast<std::size_t>(capacidad) >
            std::numeric_limits<std::size_t>::max() / static_cast<std::size_t>(capacidad))
        throw std::invalid_argument("Capacidad de usuarios invalida");
    try {
        ids_ = new int[capacidad_]();
        nombres_ = new char *[capacidad_]();
        matriz_ = new unsigned char[static_cast<std::size_t>(capacidad_) * capacidad_]();
    } catch (...) {
        delete[] ids_;
        delete[] nombres_;
        delete[] matriz_;
        throw;
    }
}
AdminAmistades::~AdminAmistades() {
    for (int i = 0; i < cantidad_; ++i) {
        MonitorRecursos::registrarIteracion(1);
        delete[] nombres_[i];
    }
    delete[] nombres_;
    delete[] ids_;
    delete[] matriz_;
}
std::size_t AdminAmistades::celda(int fila, int columna) const {
    return static_cast<std::size_t>(fila) * capacidad_ + columna;
}
bool AdminAmistades::posicionesValidas(int primero, int segundo) const {
    return primero >= 0 && segundo >= 0 && primero != segundo;
}
bool AdminAmistades::nombresIguales(const char *primero, const char *segundo) const {
    if (!primero || !segundo)
        return false;
    int i = 0;
    do {
        MonitorRecursos::registrarIteracion(1);

        if (primero[i] != segundo[i])
            return false;
    } while (primero[i++] != '\0');
    return true;
}
int AdminAmistades::buscarUsuario(int id) const {
    for (int i = 0; i < cantidad_; ++i) {
        MonitorRecursos::registrarIteracion(1);

        if (ids_[i] == id)
            return i;
    }
    return -1;
}
int AdminAmistades::buscarUsuario(const char *nombre) const {
    if (!nombre)
        return -1;
    for (int i = 0; i < cantidad_; ++i) {
        MonitorRecursos::registrarIteracion(1);

        if (nombresIguales(nombres_[i], nombre))
            return i;
    }
    return -1;
}
bool AdminAmistades::cargarUsuario(int id, const char *nombre) {
    if (id <= 0 || !nombre || !nombre[0] || buscarUsuario(id) >= 0 || buscarUsuario(nombre) >= 0)
        return false;
    std::size_t longitud = 0;
    bool contenido = false;
    while (nombre[longitud]) {
        MonitorRecursos::registrarIteracion(1);

        if (nombre[longitud] != ' ' && nombre[longitud] != '\t' && nombre[longitud] != '\r' &&
            nombre[longitud] != '\n')
            contenido = true;
        ++longitud;
    }
    if (!contenido)
        return false;
    if (cantidad_ == capacidad_)
        ampliar();
    char *copia = new char[longitud + 1];
    for (std::size_t i = 0; i <= longitud; ++i) {
        MonitorRecursos::registrarIteracion(1);

        copia[i] = nombre[i];
    }
    ids_[cantidad_] = id;
    nombres_[cantidad_++] = copia;
    bytesNombres_ += longitud + 1;
    return true;
}
void AdminAmistades::ampliar() {
    if (capacidad_ > std::numeric_limits<int>::max() / 2)
        throw std::length_error("Demasiados usuarios");
    int nueva = capacidad_ * 2;
    if (static_cast<std::size_t>(nueva) > std::numeric_limits<std::size_t>::max() / nueva)
        throw std::length_error("Matriz demasiado grande");
    int *ids = nullptr;
    char **nombres = nullptr;
    unsigned char *matriz = nullptr;
    try {
        ids = new int[nueva]();
        nombres = new char *[nueva]();
        matriz = new unsigned char[static_cast<std::size_t>(nueva) * nueva]();
    } catch (...) {
        delete[] ids;
        delete[] nombres;
        delete[] matriz;
        throw;
    }
    for (int i = 0; i < cantidad_; ++i) {
        MonitorRecursos::registrarIteracion(1);

        ids[i] = ids_[i];
        nombres[i] = nombres_[i];
        for (int j = 0; j < cantidad_; ++j) {
            MonitorRecursos::registrarIteracion(1);

            matriz[static_cast<std::size_t>(i) * nueva + j] = matriz_[celda(i, j)];
        }
    }
    delete[] ids_;
    delete[] nombres_;
    delete[] matriz_;
    ids_ = ids;
    nombres_ = nombres;
    matriz_ = matriz;
    capacidad_ = nueva;
}
bool AdminAmistades::cargarAmistad(int primero, int segundo) {
    int a = buscarUsuario(primero), b = buscarUsuario(segundo);
    if (!posicionesValidas(a, b))
        return false;
    matriz_[celda(a, b)] = matriz_[celda(b, a)] = 1;
    return true;
}
bool AdminAmistades::registrarSolicitud(int emisor, int receptor) {
    int a = buscarUsuario(emisor), b = buscarUsuario(receptor);
    if (!posicionesValidas(a, b) || matriz_[celda(a, b)] != 0 || matriz_[celda(b, a)] != 0)
        return false;
    matriz_[celda(a, b)] = 2;
    return true;
}
bool AdminAmistades::cargarSolicitud(int emisor, int receptor) {
    return registrarSolicitud(emisor, receptor);
}
int AdminAmistades::getCantidadUsuarios() const {
    return cantidad_;
}
int AdminAmistades::getIdEnPosicion(int posicion) const {
    return posicion >= 0 && posicion < cantidad_ ? ids_[posicion] : -1;
}
const char *AdminAmistades::getNombre(int id) const {
    int posicion = buscarUsuario(id);
    return posicion >= 0 ? nombres_[posicion] : nullptr;
}
bool AdminAmistades::sonAmigos(int primero, int segundo) const {
    int a = buscarUsuario(primero), b = buscarUsuario(segundo);
    return posicionesValidas(a, b) && matriz_[celda(a, b)] == 1;
}
bool AdminAmistades::haySolicitud(int emisor, int receptor) const {
    int a = buscarUsuario(emisor), b = buscarUsuario(receptor);
    return posicionesValidas(a, b) && matriz_[celda(a, b)] == 2;
}
bool AdminAmistades::envioDeSolicitud(int emisor, int receptor) {
    bool ok = registrarSolicitud(emisor, receptor);
    if (ok)
        cambiosPendientes_ = true;
    return ok;
}
bool AdminAmistades::envioDeSolicitud(int emisor, const char *nombreReceptor) {
    int posicion = buscarUsuario(nombreReceptor);
    return posicion >= 0 && envioDeSolicitud(emisor, ids_[posicion]);
}
bool AdminAmistades::aceptarSolicitud(int receptor, int emisor) {
    int a = buscarUsuario(emisor), b = buscarUsuario(receptor);
    if (!posicionesValidas(a, b) || matriz_[celda(a, b)] != 2)
        return false;
    matriz_[celda(a, b)] = matriz_[celda(b, a)] = 1;
    cambiosPendientes_ = true;
    return true;
}
bool AdminAmistades::rechazarSolicitud(int receptor, int emisor) {
    int a = buscarUsuario(emisor), b = buscarUsuario(receptor);
    if (!posicionesValidas(a, b) || matriz_[celda(a, b)] != 2)
        return false;
    matriz_[celda(a, b)] = 0;
    cambiosPendientes_ = true;
    return true;
}
int AdminAmistades::contarAmigosComunes(int primero, int segundo) const {
    int a = buscarUsuario(primero), b = buscarUsuario(segundo), total = 0;
    if (!posicionesValidas(a, b))
        return 0;
    for (int i = 0; i < cantidad_; ++i) {
        MonitorRecursos::registrarIteracion(1);

        if (matriz_[celda(a, i)] == 1 && matriz_[celda(b, i)] == 1)
            ++total;
    }
    return total;
}
int AdminAmistades::sugerirAmigos(int usuario, int k, int *ids, int *comunes) const {
    int activo = buscarUsuario(usuario);
    if (activo < 0 || k < 1 || k > 20 || !ids || !comunes || ids == comunes)
        return -1;
    int *conteos = new int[cantidad_]();
    for (int amigo = 0; amigo < cantidad_; ++amigo) {
        MonitorRecursos::registrarIteracion(1);

        if (matriz_[celda(activo, amigo)] != 1)
            continue;
        for (int candidato = 0; candidato < cantidad_; ++candidato) {
            MonitorRecursos::registrarIteracion(1);

            if (matriz_[celda(amigo, candidato)] == 1)
                ++conteos[candidato];
        }
    }
    int usados = 0;
    for (int candidato = 0; candidato < cantidad_; ++candidato) {
        MonitorRecursos::registrarIteracion(1);

        if (candidato == activo || matriz_[celda(activo, candidato)] != 0 ||
            matriz_[celda(candidato, activo)] != 0)
            continue;
        int coincidencias = conteos[candidato];
        if (coincidencias == 0)
            continue;
        int lugar = 0;
        while (lugar < usados) {
            MonitorRecursos::registrarIteracion(1);

            if (coincidencias > comunes[lugar] ||
                (coincidencias == comunes[lugar] && ids_[candidato] < ids[lugar]))
                break;
            ++lugar;
        }
        if (lugar >= k)
            continue;
        int ultimo = usados < k ? usados : k - 1;
        for (int i = ultimo; i > lugar; --i) {
            MonitorRecursos::registrarIteracion(1);

            ids[i] = ids[i - 1];
            comunes[i] = comunes[i - 1];
        }
        ids[lugar] = ids_[candidato];
        comunes[lugar] = coincidencias;
        if (usados < k)
            ++usados;
    }
    delete[] conteos;
    return usados;
}
void AdminAmistades::listarSolicitudes(int receptor, std::ostream &salida) const {
    int activo = buscarUsuario(receptor), cantidad = 0;
    if (activo < 0) {
        salida << "Usuario inexistente.\n";
        return;
    }
    for (int i = 0; i < cantidad_; ++i) {
        MonitorRecursos::registrarIteracion(1);

        if (matriz_[celda(i, activo)] == 2) {
            salida << ids_[i] << " - " << nombres_[i] << '\n';
            ++cantidad;
        }
    }
    if (!cantidad)
        salida << "No tienes solicitudes recibidas.\n";
}
void AdminAmistades::mostrarMenu(int usuario, std::istream &entrada, std::ostream &salida,
                                 void (*alTerminar)(void *), void *contexto) {
    MonitorRecursos::registrarComponente(2);
    MemoriaTemporal locales(sizeof(usuario) + sizeof(int) + 5 * sizeof(void *));
    if (buscarUsuario(usuario) < 0) {
        salida << "Usuario inexistente.\n";
        return;
    }
    int opcion = -1;
    while (opcion != 0 && entrada && salida) {
        MonitorRecursos::registrarIteracion(1);

        salida << "\nAMISTADES\n1. Enviar solicitud\n2. Ver recibidas\n"
                  "3. Aceptar solicitud\n4. Rechazar solicitud\n0. Volver\nOpcion: ";
        if (!leerEntero(entrada, opcion)) {
            salida << "Ingresa un numero.\n";
            continue;
        }
        if (opcion == 1) {
            char nombre[1025];
            salida << "Nombre de usuario: ";
            if (!entrada.getline(nombre, sizeof nombre)) {
                if (!entrada.eof()) {
                    entrada.clear();
                    entrada.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                salida << "Nombre demasiado largo o entrada finalizada.\n";
                continue;
            }
            salida
                << (envioDeSolicitud(usuario, nombre)
                        ? "Solicitud enviada.\n"
                        : "No se puede enviar: revisa usuario, amistad o solicitud pendiente.\n");
        } else if (opcion == 2)
            listarSolicitudes(usuario, salida);
        else if (opcion == 3 || opcion == 4) {
            listarSolicitudes(usuario, salida);
            int emisor = 0;
            salida << "ID del emisor (0 para volver): ";
            if (!leerEntero(entrada, emisor) || emisor == 0)
                continue;
            bool ok = opcion == 3 ? aceptarSolicitud(usuario, emisor)
                                  : rechazarSolicitud(usuario, emisor);
            salida << (ok ? "Solicitud actualizada.\n" : "No existe esa solicitud recibida.\n");
        } else if (opcion != 0)
            salida << "Opcion invalida.\n";
        if (opcion != 0 && alTerminar)
            alTerminar(contexto);
    }
}
void AdminAmistades::mostrarMenuSugerencias(int usuario, std::istream &entrada,
                                            std::ostream &salida, void (*alTerminar)(void *),
                                            void *contexto) {
    int k = 0, ids[20], comunes[20];
    MonitorRecursos::registrarComponente(2);
    MemoriaTemporal locales(sizeof(ids) + sizeof(comunes) + 4 * sizeof(int) + 5 * sizeof(void *));
    salida << "Cantidad de sugerencias (1 a 20): ";
    if (!leerEntero(entrada, k)) {
        salida << "Cantidad invalida.\n";
        return;
    }
    int total = sugerirAmigos(usuario, k, ids, comunes);
    if (alTerminar)
        alTerminar(contexto);
    if (total < 0) {
        salida << "Usuario o cantidad invalida.\n";
        return;
    }
    if (!total) {
        salida << "No hay sugerencias disponibles.\n";
        return;
    }
    for (int i = 0; i < total; ++i) {
        MonitorRecursos::registrarIteracion(1);

        salida << i + 1 << ". " << getNombre(ids[i]) << " (" << comunes[i] << " en comun)\n";
    }
    int seleccion = 0;
    salida << "Numero de sugerencia para enviar solicitud (0 para volver): ";
    if (!leerEntero(entrada, seleccion) || seleccion == 0)
        return;
    if (seleccion < 1 || seleccion > total) {
        salida << "Seleccion invalida.\n";
        return;
    }
    salida << (envioDeSolicitud(usuario, ids[seleccion - 1])
                   ? "Solicitud enviada.\n"
                   : "No fue posible enviar la solicitud.\n");
    if (alTerminar)
        alTerminar(contexto);
}
void AdminAmistades::reiniciarIteraciones() const {
    MonitorRecursos::reiniciarModulo(1);
}
unsigned long long AdminAmistades::getIteraciones() const {
    return MonitorRecursos::getIteracionesModulo(1);
}
std::size_t AdminAmistades::getMemoriaReservada() const {
    return sizeof(*this) +
           static_cast<std::size_t>(capacidad_) *
               (sizeof(int) + sizeof(char *) + static_cast<std::size_t>(capacidad_)) +
           bytesNombres_;
}

bool AdminAmistades::hayCambios() const {
    return cambiosPendientes_;
}
void AdminAmistades::confirmarGuardado() {
    cambiosPendientes_ = false;
}
unsigned char AdminAmistades::getRelacion(int fila, int columna) const {
    if (fila < 0 || fila >= cantidad_ || columna < 0 || columna >= cantidad_)
        return 0;
    return matriz_[celda(fila, columna)];
}
