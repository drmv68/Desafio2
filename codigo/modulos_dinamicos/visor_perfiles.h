#ifndef VISOR_PERFILES_H
#define VISOR_PERFILES_H
#include <cstddef>
#include <iosfwd>
#include "../core/gestor_archivos.h"
#include "admin_amistades.h"

class VisorPerfiles {
  public:
    VisorPerfiles();
    bool buscarPerfilPorNombre(const GestorArchivos &base, const char *nombre, char *destino,
                               std::size_t capacidad = 100) const;
    void mostrarCarrusel(const GestorArchivos &base, const AdminAmistades &red,
                         const char *idPropio, const char *idPerfil, std::istream &entrada,
                         std::ostream &salida, void (*alTerminar)(void *) = nullptr,
                         void *contexto = nullptr) const;
    void reiniciarIteraciones() const;
    unsigned long long getIteraciones() const;
    std::size_t getMemoriaReservada() const;
};
#endif
