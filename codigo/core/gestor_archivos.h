#ifndef GESTOR_ARCHIVOS_H
#define GESTOR_ARCHIVOS_H

#include <cstddef>
#include "estructuras_base.h"
#include "monitor_recursos.h"

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

    void reiniciarIteraciones() const;
    unsigned long long getIteraciones() const;
    std::size_t getMemoriaReservada() const;
};

#endif
