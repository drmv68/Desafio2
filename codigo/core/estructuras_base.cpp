#include "estructuras_base.h"
#include <cstring>

// ============================================================
// core/estructuras_base.cpp
// Implementacion de UsuarioLigero (Fase 1). EstructuraMensaje
// sigue pendiente para cuando lleguemos al modulo 6.
// ============================================================

// Funcion auxiliar INTERNA de este modulo (no esta en el .h,
// por lo tanto nadie fuera de este archivo puede usarla; asi
// evitamos repetir la misma logica de copia en los 3 setters).
static void copiarTexto(char*& destino, const char* origen) {
    delete[] destino;
    // [Debugger] Si destino ya apuntaba a algo (una carga anterior),
    // lo liberamos antes de pedir memoria nueva. delete[] sobre
    // nullptr no hace nada, es seguro llamarlo siempre.

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
    // [Debugger] Constructor por defecto: los 3 punteros arrancan en
    // nullptr. Esto es clave porque GestorArchivos va a hacer
    // "new UsuarioLigero[totalUsuarios]", y C++ llama este
    // constructor automaticamente para CADA casillero del arreglo.
}

UsuarioLigero::UsuarioLigero(const UsuarioLigero& otro) {
    idUsuario = nullptr;
    contrasena = nullptr;
    nombreUsuario = nullptr;
    copiarTexto(idUsuario, otro.idUsuario);
    copiarTexto(contrasena, otro.contrasena);
    copiarTexto(nombreUsuario, otro.nombreUsuario);
    // [Debugger] Constructor de copia: copia PROFUNDA (deep copy).
    // Si solo hicieramos "idUsuario = otro.idUsuario;" (copia
    // superficial), las dos instancias apuntarian al MISMO bloque
    // de memoria y el destructor lo liberaria dos veces -> crash.
}

UsuarioLigero::~UsuarioLigero() {
    delete[] idUsuario;
    delete[] contrasena;
    delete[] nombreUsuario;
    // [Debugger] Se libera cada campo. Como GestorArchivos guarda
    // estos objetos en un arreglo dinamico, este destructor se
    // dispara solo, una vez por casillero, cuando se haga el
    // "delete[] arregloUsuariosLigeros;" en ~GestorArchivos().
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
