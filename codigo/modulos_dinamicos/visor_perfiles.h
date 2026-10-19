#ifndef VISOR_PERFILES_H
#define VISOR_PERFILES_H

#include <cstddef>
#include "../core/gestor_archivos.h"
#include "../core/estructuras_base.h"
#include "admin_amistades.h"

// ============================================================
// modulos_dinamicos/visor_perfiles.h
// Modulo 4 del analisis: "Visitar perfil de un amigo".
// Busca un perfil por nombre, decide si mostrar tambien las
// fotos de amigos (segun AdminAmistades::sonAmigos), y permite
// dar like a una foto. Nunca toca archivos directamente: todo
// pasa por GestorArchivos (leerPerfilCompleto/guardarCambioDeLike).
// ============================================================

class VisorPerfiles {
private:
    mutable unsigned long long iteracionesDadas;

public:
    VisorPerfiles();

    // Busca por nombre en el indice de GestorArchivos. Si lo encuentra,
    // retorna true y deja el id del perfil encontrado en idEncontradoDestino
    // (debe venir con espacio para al menos 100 caracteres).
    bool buscarPerfilPorNombre(const GestorArchivos& baseDatos, const char* nombreBuscado,
                                char* idEncontradoDestino) const;

    // Muestra en consola las fotos visibles del perfil con ese id: las
    // publicas siempre, y ademas las de amigos si idPropio y idEncontrado
    // son amigos segun redDeAmistades.
    void mostrarCarrusel(const GestorArchivos& baseDatos, const AdminAmistades& redDeAmistades,
                          const char* idPropio, const char* idEncontrado) const;

    // Suma un like a una foto puntual de ese perfil y pide a GestorArchivos
    // que lo deje guardado en el archivo.
    // OJO (limitacion conocida, ver mensaje aparte): FotoConLikes solo guarda
    // un CONTADOR de likes, no la lista de quien le dio like. Por eso esto
    // suma, pero no puede "quitar" el like de una persona en particular
    // todavia (eso necesitaria guardar, por cada foto, los ids de quienes
    // ya le dieron like).
    // (GestorArchivos::guardarCambioDeLike es const porque no cambia sus
    // propios atributos, solo reescribe el archivo, asi que basta una
    // referencia const aqui tambien.)
    bool darLike(const GestorArchivos& baseDatos, const char* idDelPerfil,
                 const char* nombreDeLaFoto) const;

    void reiniciarIteraciones() const;
    unsigned long long getIteraciones() const;
    std::size_t getMemoriaReservada() const;
};

#endif // VISOR_PERFILES_H
