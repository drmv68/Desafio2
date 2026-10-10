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
// FASE MACRO: solo el cascaron de la clase. Sin logica interna
// todavia, eso se arma cuando definamos el formato de archivo.
// ============================================================

class GestorArchivos {
    // Futuros metodos: leerPerfilCompleto(), actualizarLikes(), escribirNuevoMensaje()
    // Futuras funciones auxiliares optimizadas para lectura.
};

#endif // GESTOR_ARCHIVOS_H
