#ifndef GESTOR_ARCHIVOS_H
#define GESTOR_ARCHIVOS_H

#include <cstddef>
#include "estructuras_base.h"
#include "monitor_recursos.h"

class AdminAmistades;
class GestorMensajes;

class GestorArchivos {
private:
    UsuarioLigero* arregloUsuariosLigeros;
    int totalUsuarios;
    mutable unsigned long long iteracionesDadas;

public:
    GestorArchivos();
    ~GestorArchivos();

    bool cargarIndiceUsuarios(const char* rutaArchivo);

    int getTotalUsuarios() const;
    const UsuarioLigero& getUsuarioLigero(int indice) const;

    bool leerPerfilCompleto(const char* idUsuario, PerfilCompleto& perfilDestino) const;
    bool guardarCambioDeLike(const char* idUsuario, const char* nombreDeArchivo,
                              int nuevaCantidadDeLikes) const;

    bool cargarAmistades(const char* rutaArchivo, AdminAmistades& red) const;
    bool cargarSolicitudes(const char* rutaArchivo, AdminAmistades& red) const;
    bool cargarMensajes(const char* rutaArchivo, const AdminAmistades& red, GestorMensajes& moduloMensajes) const;

    void reiniciarIteraciones() const;
    unsigned long long getIteraciones() const;
    std::size_t getMemoriaReservada() const;
};

#endif
