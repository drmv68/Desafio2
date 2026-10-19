#include "monitor_recursos.h"
#include "estructuras_base.h"
#include <chrono>
#include <stdexcept>
#include <limits>
#include <utility>

namespace {
std::size_t longitud(const char *texto) {
    std::size_t n = 0;
    if (texto)
        while (texto[n]) {
            MonitorRecursos::registrarIteracion(0);
            ++n;
        }
    return n;
}
void copiarTexto(char *&destino, const char *origen) {
    // Reservar antes de liberar permite setNombre(getNombre()) y protege ante bad_alloc.
    char *copia = nullptr;
    if (origen) {
        std::size_t n = longitud(origen);
        copia = new char[n + 1];
        for (std::size_t i = 0; i <= n; ++i) {
            MonitorRecursos::registrarIteracion(0);
            copia[i] = origen[i];
        }
    }
    delete[] destino;
    destino = copia;
}
bool iguales(const char *a, const char *b) {
    if (!a || !b)
        return a == b;
    while (*a && *a == *b) {
        MonitorRecursos::registrarIteracion(0);
        ++a;
        ++b;
    }
    return *a == *b;
}
} // namespace
Fecha::Fecha(int dia, int mes, int anio) : valor_(0) {
    if (anio < 1 || anio > 9999 || mes < 1 || mes > 12)
        throw std::invalid_argument("Fecha invalida");
    const int dias[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int limite = dias[mes - 1];
    if (mes == 2 && anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0))
        ++limite;
    if (dia < 1 || dia > limite)
        throw std::invalid_argument("Fecha invalida");
    valor_ = anio * 10000 + mes * 100 + dia;
}
Fecha Fecha::actual() {
    MonitorRecursos::registrarComponente(4);
    // Hora de Colombia (UTC-5), sin una fecha de demostracion fija ni comandos externos.
    using namespace std::chrono;
    long long dias = duration_cast<hours>(system_clock::now().time_since_epoch()).count();
    dias = (dias - 5) / 24;
    int anio = 1970;
    while (dias >= 365 + (anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0))) {
        MonitorRecursos::registrarIteracion(0);
        dias -= 365 + (anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0));
        ++anio;
    }
    const int meses[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int mes = 1;
    while (true) {
        MonitorRecursos::registrarIteracion(0);
        int cantidad =
            meses[mes - 1] + (mes == 2 && anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0));
        if (dias < cantidad)
            break;
        dias -= cantidad;
        ++mes;
    }
    return Fecha(static_cast<int>(dias) + 1, mes, anio);
}
int Fecha::getDia() const {
    return valor_ % 100;
}
int Fecha::getMes() const {
    return valor_ / 100 % 100;
}
int Fecha::getAnio() const {
    return valor_ / 10000;
}
bool Fecha::operator==(const Fecha &otra) const {
    return valor_ == otra.valor_;
}
bool Fecha::operator<=(const Fecha &otra) const {
    return valor_ <= otra.valor_;
}

UsuarioLigero::UsuarioLigero() : idUsuario(nullptr), contrasena(nullptr), nombreUsuario(nullptr) {
}
UsuarioLigero::UsuarioLigero(const UsuarioLigero &otro) : UsuarioLigero() {
    try {
        *this = otro;
    } catch (...) {
        delete[] idUsuario;
        delete[] contrasena;
        delete[] nombreUsuario;
        throw;
    }
}
UsuarioLigero &UsuarioLigero::operator=(const UsuarioLigero &otro) {
    if (this != &otro) {
        copiarTexto(idUsuario, otro.idUsuario);
        copiarTexto(contrasena, otro.contrasena);
        copiarTexto(nombreUsuario, otro.nombreUsuario);
    }
    return *this;
}
UsuarioLigero::~UsuarioLigero() {
    delete[] idUsuario;
    delete[] contrasena;
    delete[] nombreUsuario;
}
void UsuarioLigero::setIdUsuario(const char *valor) {
    copiarTexto(idUsuario, valor);
}
void UsuarioLigero::setContrasena(const char *valor) {
    copiarTexto(contrasena, valor);
}
void UsuarioLigero::setNombreUsuario(const char *valor) {
    copiarTexto(nombreUsuario, valor);
}
const char *UsuarioLigero::getIdUsuario() const {
    return idUsuario;
}
const char *UsuarioLigero::getContrasena() const {
    return contrasena;
}
const char *UsuarioLigero::getNombreUsuario() const {
    return nombreUsuario;
}
std::size_t UsuarioLigero::getMemoriaReservada() const {
    return sizeof(*this) + (idUsuario ? longitud(idUsuario) + 1 : 0) +
           (contrasena ? longitud(contrasena) + 1 : 0) +
           (nombreUsuario ? longitud(nombreUsuario) + 1 : 0);
}
FotoConLikes::FotoConLikes()
    : nombreDeArchivo(nullptr), cantidadDeLikes(0), indiceAlbum(-1), likeEnSesion(false),
      fechaCaptura(), tamano(0) {
}
FotoConLikes::FotoConLikes(const FotoConLikes &otra) : FotoConLikes() {
    *this = otra;
}
FotoConLikes &FotoConLikes::operator=(const FotoConLikes &otra) {
    if (this != &otra) {
        copiarTexto(nombreDeArchivo, otra.nombreDeArchivo);
        cantidadDeLikes = otra.cantidadDeLikes;
        indiceAlbum = otra.indiceAlbum;
        likeEnSesion = otra.likeEnSesion;
        fechaCaptura = otra.fechaCaptura;
        tamano = otra.tamano;
    }
    return *this;
}
FotoConLikes::~FotoConLikes() {
    delete[] nombreDeArchivo;
}
void FotoConLikes::setNombreDeArchivo(const char *v) {
    copiarTexto(nombreDeArchivo, v);
}
void FotoConLikes::setCantidadDeLikes(int v) {
    if (v < 0)
        throw std::invalid_argument("Likes negativos");
    cantidadDeLikes = v;
}
void FotoConLikes::setIndiceAlbum(int v) {
    indiceAlbum = v;
}
void FotoConLikes::setFechaCaptura(const Fecha &v) {
    fechaCaptura = v;
}
void FotoConLikes::setTamano(std::size_t v) {
    tamano = v;
}
void FotoConLikes::setLikeEnSesion(bool v) {
    likeEnSesion = v;
}
const char *FotoConLikes::getNombreDeArchivo() const {
    return nombreDeArchivo;
}
int FotoConLikes::getCantidadDeLikes() const {
    return cantidadDeLikes;
}
int FotoConLikes::getIndiceAlbum() const {
    return indiceAlbum;
}
const Fecha &FotoConLikes::getFechaCaptura() const {
    return fechaCaptura;
}
std::size_t FotoConLikes::getTamano() const {
    return tamano;
}
bool FotoConLikes::getLikeEnSesion() const {
    return likeEnSesion;
}
std::size_t FotoConLikes::getMemoriaReservada() const {
    return sizeof(*this) + (nombreDeArchivo ? longitud(nombreDeArchivo) + 1 : 0);
}
Album::Album() : nombre(nullptr), privacidad(1) {
}
Album::Album(const Album &otro) : Album() {
    *this = otro;
}
Album &Album::operator=(const Album &otro) {
    if (this != &otro) {
        copiarTexto(nombre, otro.nombre);
        privacidad = otro.privacidad;
    }
    return *this;
}
Album::~Album() {
    delete[] nombre;
}
void Album::configurar(const char *n, int p) {
    if (!n || !n[0] || p < 0 || p > 2)
        throw std::invalid_argument("Album invalido");
    copiarTexto(nombre, n);
    privacidad = p;
}
const char *Album::getNombre() const {
    return nombre;
}
int Album::getPrivacidad() const {
    return privacidad;
}
std::size_t Album::getMemoriaReservada() const {
    return sizeof(*this) + (nombre ? longitud(nombre) + 1 : 0);
}
PerfilCompleto::PerfilCompleto()
    : rutaCarpetaDeFotos(nullptr), fotos(nullptr), cantidadDeFotos(0), albumes(nullptr),
      cantidadDeAlbumes(0), capacidadDeAlbumes(0), correo(nullptr), ciudad(nullptr), pais(nullptr),
      nacimiento(), inscripcion() {
}
PerfilCompleto::PerfilCompleto(const PerfilCompleto &otro) : PerfilCompleto() {
    try {
        setRutaCarpetaDeFotos(otro.rutaCarpetaDeFotos);
        inicializarFotos(otro.cantidadDeFotos);
        if (otro.correo)
            setDatosUsuario(otro.correo, otro.ciudad, otro.pais, otro.nacimiento, otro.inscripcion);
        for (int i = 0; i < otro.cantidadDeAlbumes; ++i) {
            MonitorRecursos::registrarIteracion(0);
            incorporarAlbum(otro.albumes[i].getNombre(), otro.albumes[i].getPrivacidad());
        }
        for (int i = 0; i < cantidadDeFotos; ++i) {
            MonitorRecursos::registrarIteracion(0);
            fotos[i] = otro.fotos[i];
        }
    } catch (...) {
        delete[] rutaCarpetaDeFotos;
        delete[] fotos;
        delete[] albumes;
        delete[] correo;
        delete[] ciudad;
        delete[] pais;
        throw;
    }
}
PerfilCompleto &PerfilCompleto::operator=(const PerfilCompleto &otro) {
    if (this != &otro) {
        PerfilCompleto copia(otro);
        std::swap(rutaCarpetaDeFotos, copia.rutaCarpetaDeFotos);
        std::swap(fotos, copia.fotos);
        std::swap(cantidadDeFotos, copia.cantidadDeFotos);
        std::swap(albumes, copia.albumes);
        std::swap(cantidadDeAlbumes, copia.cantidadDeAlbumes);
        std::swap(capacidadDeAlbumes, copia.capacidadDeAlbumes);
        std::swap(correo, copia.correo);
        std::swap(ciudad, copia.ciudad);
        std::swap(pais, copia.pais);
        std::swap(nacimiento, copia.nacimiento);
        std::swap(inscripcion, copia.inscripcion);
    }
    return *this;
}
PerfilCompleto::~PerfilCompleto() {
    delete[] rutaCarpetaDeFotos;
    delete[] fotos;
    delete[] albumes;
    delete[] correo;
    delete[] ciudad;
    delete[] pais;
}
void PerfilCompleto::setRutaCarpetaDeFotos(const char *v) {
    copiarTexto(rutaCarpetaDeFotos, v);
}
void PerfilCompleto::inicializarFotos(int cantidad) {
    if (cantidad < 0)
        throw std::invalid_argument("Cantidad de fotos negativa");
    FotoConLikes *nuevas = cantidad ? new FotoConLikes[cantidad] : nullptr;
    Album *nuevos = nullptr;
    try {
        nuevos = cantidad ? new Album[cantidad] : nullptr;
    } catch (...) {
        delete[] nuevas;
        throw;
    }
    delete[] fotos;
    delete[] albumes;
    fotos = nuevas;
    albumes = nuevos;
    cantidadDeFotos = cantidad;
    cantidadDeAlbumes = 0;
    capacidadDeAlbumes = cantidad;
}
int PerfilCompleto::incorporarAlbum(const char *nombre, int privacidad) {
    for (int i = 0; i < cantidadDeAlbumes; ++i) {
        MonitorRecursos::registrarIteracion(0);
        if (iguales(albumes[i].getNombre(), nombre)) {
            if (albumes[i].getPrivacidad() != privacidad)
                throw std::invalid_argument("Privacidad contradictoria en album");
            return i;
        }
    }
    if (cantidadDeAlbumes == capacidadDeAlbumes) {
        if (capacidadDeAlbumes > std::numeric_limits<int>::max() / 2)
            throw std::length_error("Demasiados albumes");
        int nueva = capacidadDeAlbumes ? capacidadDeAlbumes * 2 : 2;
        Album *ampliados = new Album[nueva];
        try {
            for (int i = 0; i < cantidadDeAlbumes; ++i) {
                MonitorRecursos::registrarIteracion(0);
                ampliados[i] = albumes[i];
            }
        } catch (...) {
            delete[] ampliados;
            throw;
        }
        delete[] albumes;
        albumes = ampliados;
        capacidadDeAlbumes = nueva;
    }
    albumes[cantidadDeAlbumes].configurar(nombre, privacidad);
    return cantidadDeAlbumes++;
}
const char *PerfilCompleto::getRutaCarpetaDeFotos() const {
    return rutaCarpetaDeFotos;
}
int PerfilCompleto::getCantidadDeFotos() const {
    return cantidadDeFotos;
}
int PerfilCompleto::getCantidadDeAlbumes() const {
    return cantidadDeAlbumes;
}
FotoConLikes &PerfilCompleto::getFotoEnPosicion(int i) {
    if (i < 0 || i >= cantidadDeFotos)
        throw std::out_of_range("Foto inexistente");
    return fotos[i];
}
const FotoConLikes &PerfilCompleto::getFotoEnPosicion(int i) const {
    if (i < 0 || i >= cantidadDeFotos)
        throw std::out_of_range("Foto inexistente");
    return fotos[i];
}
const Album &PerfilCompleto::getAlbum(int i) const {
    if (i < 0 || i >= cantidadDeAlbumes)
        throw std::out_of_range("Album inexistente");
    return albumes[i];
}
std::size_t PerfilCompleto::getMemoriaReservada() const {
    std::size_t bytes = sizeof(*this) + (rutaCarpetaDeFotos ? longitud(rutaCarpetaDeFotos) + 1 : 0);
    for (int i = 0; i < cantidadDeFotos; ++i) {
        MonitorRecursos::registrarIteracion(0);
        bytes += fotos[i].getMemoriaReservada();
    }
    bytes += static_cast<std::size_t>(capacidadDeAlbumes) * sizeof(Album);
    for (int i = 0; i < cantidadDeAlbumes; ++i) {
        MonitorRecursos::registrarIteracion(0);
        bytes += albumes[i].getMemoriaReservada() - sizeof(Album);
    }
    bytes += (correo ? longitud(correo) + 1 : 0) + (ciudad ? longitud(ciudad) + 1 : 0) +
             (pais ? longitud(pais) + 1 : 0);
    return bytes;
}

void PerfilCompleto::setDatosUsuario(const char *c, const char *ci, const char *p, const Fecha &n,
                                     const Fecha &i) {
    if (!c || !*c || !ci || !*ci || !p || !*p || !(n <= i))
        throw std::invalid_argument("Datos de usuario invalidos");
    copiarTexto(correo, c);
    copiarTexto(ciudad, ci);
    copiarTexto(pais, p);
    nacimiento = n;
    inscripcion = i;
}
const char *PerfilCompleto::getCorreo() const {
    return correo;
}
const char *PerfilCompleto::getCiudad() const {
    return ciudad;
}
const char *PerfilCompleto::getPais() const {
    return pais;
}
const Fecha &PerfilCompleto::getNacimiento() const {
    return nacimiento;
}
const Fecha &PerfilCompleto::getInscripcion() const {
    return inscripcion;
}
