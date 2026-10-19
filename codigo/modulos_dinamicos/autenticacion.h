#ifndef AUTENTICACION_H
#define AUTENTICACION_H
#include <cstddef>
#include <iosfwd>
#include <random>
#include "../core/gestor_archivos.h"
#include "admin_amistades.h"

class Autenticacion {
    char *idDelUsuarioActivo;
    int amigosMostrados[3];
    int cantidadMostrada;
    bool actualizacionUsada;
    std::mt19937 generador;

  public:
    Autenticacion();
    ~Autenticacion();
    Autenticacion(const Autenticacion &) = delete;
    Autenticacion &operator=(const Autenticacion &) = delete;
    bool iniciarSesion(const GestorArchivos &baseDatos);
    bool iniciarSesion(const GestorArchivos &baseDatos, std::istream &entrada,
                       std::ostream &salida);
    const char *getIdDelUsuarioActivo() const;
    bool mostrarMejoresAmigos(const GestorArchivos &baseDatos, const AdminAmistades &red,
                              bool actualizar, std::ostream &salida);
    int getAmigoMostrado(int posicion) const;
    void reiniciarIteraciones() const;
    unsigned long long getIteraciones() const;
    std::size_t getMemoriaReservada() const;
};
#endif
