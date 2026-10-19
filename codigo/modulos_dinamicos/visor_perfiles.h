#ifndef VISOR_PERFILES_H
#define VISOR_PERFILES_H

#include <cstddef>
#include "../core/gestor_archivos.h"
#include "../core/estructuras_base.h"
#include "admin_amistades.h"

class VisorPerfiles {
private:
    mutable unsigned long long iteracionesDadas;

public:
    VisorPerfiles();

    bool buscarPerfilPorNombre(const GestorArchivos& baseDatos, const char* nombreBuscado,
                                char* idEncontradoDestino) const;

    void mostrarCarrusel(const GestorArchivos& baseDatos, const AdminAmistades& redDeAmistades,
                          const char* idPropio, const char* idEncontrado) const;

    bool darLike(const GestorArchivos& baseDatos, const char* idDelPerfil,
                 const char* nombreDeLaFoto) const;

    void reiniciarIteraciones() const;
    unsigned long long getIteraciones() const;
    std::size_t getMemoriaReservada() const;
};

#endif
