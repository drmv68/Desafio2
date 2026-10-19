#ifndef GESTOR_ARCHIVOS_H
#define GESTOR_ARCHIVOS_H

#include <cstddef>
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

    // Mismo patron que ya usan AdminAmistades y GestorMensajes (ver sus .h),
    // para que MonitorRecursos pueda tratar a todos los modulos por igual.
    mutable unsigned long long iteracionesDadas;

public:
    GestorArchivos();
    ~GestorArchivos();

    // Fase 1: Funcionalidad I - Carga de Datos (lectura del indice)
    bool cargarIndiceUsuarios(const char* rutaArchivo);

    int getTotalUsuarios() const;
    const UsuarioLigero& getUsuarioLigero(int indice) const;

    // Fase 2: Funcionalidad IV (Visitar perfil de un amigo).
    // GestorArchivos sigue siendo el UNICO que toca archivos; VisorPerfiles
    // solo llama a estos dos metodos, nunca abre un ifstream/ofstream el mismo.
    //
    // Busca "estaticos_y_db/perfiles/<idUsuario>.txt", lo lee completo y llena
    // perfilDestino. Devuelve false si el archivo no existe o esta mal formado.
    bool leerPerfilCompleto(const char* idUsuario, PerfilCompleto& perfilDestino) const;

    // Reescribe ese mismo archivo cambiando solo la linea de esa foto puntual.
    // Devuelve false si el perfil o la foto no existen.
    bool guardarCambioDeLike(const char* idUsuario, const char* nombreDeArchivo,
                              int nuevaCantidadDeLikes) const;

    // Futuros metodos: escribirNuevoMensaje()
    // Futuras funciones auxiliares optimizadas para lectura.

    // Mismo patron que AdminAmistades::reiniciarIteraciones() / getIteraciones() /
    // getMemoriaReservada(). Daniel debe sumar aparte lo que gasten parametros y
    // variables locales vivas; esto solo cuenta lo que posee GestorArchivos.
    void reiniciarIteraciones() const;
    unsigned long long getIteraciones() const;
    std::size_t getMemoriaReservada() const;
};

#endif // GESTOR_ARCHIVOS_H
