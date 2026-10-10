#ifndef GESTOR_MENSAJES_H
#define GESTOR_MENSAJES_H

#include "../core/gestor_archivos.h"
#include "../core/estructuras_base.h"

// ============================================================
// modulos_dinamicos/gestor_mensajes.h
// Modulo 6 del analisis: "Administrar mensajes".
// Enviar mensaje, leer nuevos (no leidos) y buscar por rango de
// fecha (ambas fechas inclusive). Se apoya en EstructuraMensaje
// de core/estructuras_base.h para el filtrado por fecha.
//
// FASE MACRO: solo el cascaron de la clase. Sin logica interna
// todavia, eso se arma cuando definamos el formato de archivo
// de mensajes (contador + id + fecha).
// ============================================================

class GestorMensajes {
    // Futuros metodos: enviarMensaje(), leerNuevos(),
    // buscarPorRangoDeFechas(fechaInicio, fechaFin)
};

#endif // GESTOR_MENSAJES_H
