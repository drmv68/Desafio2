#ifndef AUTENTICACION_H
#define AUTENTICACION_H

#include <cstddef>
#include "../core/gestor_archivos.h"
#include "../core/estructuras_base.h"
#include "admin_amistades.h"

// ============================================================
// modulos_dinamicos/autenticacion.h
// Modulo 2 del analisis: "Ingreso a la plataforma y
// visualizacion de mejores amigos".
//
// No calcula "sugerencias de amigos nuevos" (eso ya es trabajo
// de AdminAmistades::sugerirAmigos/mostrarMenuSugerencias). Esta
// clase solo ordena, entre los amigos QUE YA TIENE el usuario que
// inicio sesion, cual comparte mas amigos en comun con el.
// ============================================================

class Autenticacion {
private:
    char* idDelUsuarioActivo; // id (texto) de quien inicio sesion; nullptr si nadie ha entrado
    mutable unsigned long long iteracionesDadas;

public:
    Autenticacion();
    ~Autenticacion();

    // Pide por consola id y contrasena, los compara contra el indice que ya
    // cargo GestorArchivos. Si coincide, guarda el id como "usuario activo"
    // (recuperable con getIdDelUsuarioActivo()) y retorna true.
    bool iniciarSesion(const GestorArchivos& baseDatos);

    const char* getIdDelUsuarioActivo() const;

    // Imprime cual de los amigos YA CONFIRMADOS del usuario activo comparte
    // mas amigos en comun con el (el de "mayor prioridad").
    void mostrarMejoresAmigos(const AdminAmistades& redDeAmistades) const;

    // Mismo patron que AdminAmistades/GestorMensajes/GestorArchivos.
    void reiniciarIteraciones() const;
    unsigned long long getIteraciones() const;
    std::size_t getMemoriaReservada() const;
};

#endif // AUTENTICACION_H
