#ifndef GESTOR_ARCHIVOS_H
#define GESTOR_ARCHIVOS_H

#include "estructuras_base.h"
#include "monitor_recursos.h"

// ============================================================
// core/gestor_archivos.h
// Modulo 1 del analisis: "Carga y actualizacion de datos".
// Unico modulo que habla directamente con la carpeta estatica
// (estaticos_y_db/). Unico lugar donde se planea usar memoria
// dinamica de forma intensiva para transformar los .txt en
// objetos de estructuras_base.h.
//
// FASE 1 (en aterrizaje): ya tiene cargarIndiceUsuarios() real.
// Lo demas (leerPerfilCompleto, actualizarLikes, etc.) sigue
// pendiente para las siguientes funcionalidades.
// ============================================================

class GestorArchivos {
private:
    UsuarioLigero* arregloUsuariosLigeros; // arreglo dinamico, tamano = totalUsuarios
    int totalUsuarios;

public:
    GestorArchivos();
    ~GestorArchivos();

    // Fase 1: Funcionalidad I - Carga de Datos (lectura del indice)
    bool cargarIndiceUsuarios(const char* rutaArchivo);

    int getTotalUsuarios() const;
    const UsuarioLigero& getUsuarioLigero(int indice) const;

    // Futuros metodos: leerPerfilCompleto(), actualizarLikes(), escribirNuevoMensaje()
    // Futuras funciones auxiliares optimizadas para lectura.
};

#endif // GESTOR_ARCHIVOS_H
