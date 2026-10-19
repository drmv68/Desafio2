#include "estructuras_base.h"

static void copiarTexto(char*& destino, const char* origen) {
    delete[] destino;

    if (origen == nullptr) {
        destino = nullptr;
        return;
    }

    int longitud = 0;
    while (origen[longitud] != '\0') {
        longitud++;
    }

    destino = new char[longitud + 1];
    for (int i = 0; i < longitud; i++) {
        destino[i] = origen[i];
    }
    destino[longitud] = '\0';
}

UsuarioLigero::UsuarioLigero() {
    idUsuario = nullptr;
    contrasena = nullptr;
    nombreUsuario = nullptr;
}

UsuarioLigero::UsuarioLigero(const UsuarioLigero& otro) {
    idUsuario = nullptr;
    contrasena = nullptr;
    nombreUsuario = nullptr;
    copiarTexto(idUsuario, otro.idUsuario);
    copiarTexto(contrasena, otro.contrasena);
    copiarTexto(nombreUsuario, otro.nombreUsuario);
}

UsuarioLigero::~UsuarioLigero() {
    delete[] idUsuario;
    delete[] contrasena;
    delete[] nombreUsuario;
}

void UsuarioLigero::setIdUsuario(const char* valor) {
    copiarTexto(idUsuario, valor);
}

void UsuarioLigero::setContrasena(const char* valor) {
    copiarTexto(contrasena, valor);
}

void UsuarioLigero::setNombreUsuario(const char* valor) {
    copiarTexto(nombreUsuario, valor);
}

const char* UsuarioLigero::getIdUsuario() const {
    return idUsuario;
}

const char* UsuarioLigero::getContrasena() const {
    return contrasena;
}

const char* UsuarioLigero::getNombreUsuario() const {
    return nombreUsuario;
}

FotoConLikes::FotoConLikes() {
    nombreDeArchivo = nullptr;
    cantidadDeLikes = 0;
    esPublica = false;
}

FotoConLikes::FotoConLikes(const FotoConLikes& otra) {
    nombreDeArchivo = nullptr;
    copiarTexto(nombreDeArchivo, otra.nombreDeArchivo);
    cantidadDeLikes = otra.cantidadDeLikes;
    esPublica = otra.esPublica;
}

FotoConLikes::~FotoConLikes() {
    delete[] nombreDeArchivo;
}

void FotoConLikes::setNombreDeArchivo(const char* valor) {
    copiarTexto(nombreDeArchivo, valor);
}

void FotoConLikes::setCantidadDeLikes(int valor) {
    cantidadDeLikes = valor;
}

void FotoConLikes::setEsPublica(bool valor) {
    esPublica = valor;
}

const char* FotoConLikes::getNombreDeArchivo() const {
    return nombreDeArchivo;
}

int FotoConLikes::getCantidadDeLikes() const {
    return cantidadDeLikes;
}

bool FotoConLikes::getEsPublica() const {
    return esPublica;
}

PerfilCompleto::PerfilCompleto() {
    rutaCarpetaDeFotos = nullptr;
    fotos = nullptr;
    cantidadDeFotos = 0;
}

PerfilCompleto::PerfilCompleto(const PerfilCompleto& otro) {
    rutaCarpetaDeFotos = nullptr;
    fotos = nullptr;
    cantidadDeFotos = 0;

    copiarTexto(rutaCarpetaDeFotos, otro.rutaCarpetaDeFotos);

    inicializarFotos(otro.cantidadDeFotos);
    int posicionActual = 0;
    while (posicionActual < cantidadDeFotos) {
        fotos[posicionActual].setNombreDeArchivo(otro.fotos[posicionActual].getNombreDeArchivo());
        fotos[posicionActual].setCantidadDeLikes(otro.fotos[posicionActual].getCantidadDeLikes());
        fotos[posicionActual].setEsPublica(otro.fotos[posicionActual].getEsPublica());
        posicionActual = posicionActual + 1;
    }
}

PerfilCompleto::~PerfilCompleto() {
    delete[] rutaCarpetaDeFotos;
    delete[] fotos;
}

void PerfilCompleto::setRutaCarpetaDeFotos(const char* valor) {
    copiarTexto(rutaCarpetaDeFotos, valor);
}

void PerfilCompleto::inicializarFotos(int cantidad) {
    delete[] fotos;
    cantidadDeFotos = cantidad;
    if (cantidad > 0) {
        fotos = new FotoConLikes[cantidad];
    } else {
        fotos = nullptr;
    }
}

const char* PerfilCompleto::getRutaCarpetaDeFotos() const {
    return rutaCarpetaDeFotos;
}

int PerfilCompleto::getCantidadDeFotos() const {
    return cantidadDeFotos;
}

FotoConLikes& PerfilCompleto::getFotoEnPosicion(int posicion) {
    return fotos[posicion];
}
