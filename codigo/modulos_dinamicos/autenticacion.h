#ifndef AUTENTICACION_H
#define AUTENTICACION_H

#include <cstddef>
#include "../core/gestor_archivos.h"
#include "../core/estructuras_base.h"
#include "admin_amistades.h"

class Autenticacion {
private:
    char* idDelUsuarioActivo;
    mutable unsigned long long iteracionesDadas;

public:
    Autenticacion();
    ~Autenticacion();

    bool iniciarSesion(const GestorArchivos& baseDatos);
    const char* getIdDelUsuarioActivo() const;
    void mostrarMejoresAmigos(const AdminAmistades& redDeAmistades) const;

    void reiniciarIteraciones() const;
    unsigned long long getIteraciones() const;
    std::size_t getMemoriaReservada() const;
};

#endif
