#ifndef ADMIN_AMISTADES_H
#define ADMIN_AMISTADES_H

#include <cstddef>
#include <iosfwd>

class AdminAmistades {
public:
    explicit AdminAmistades(int capacidad = 8);
    ~AdminAmistades();
    AdminAmistades(const AdminAmistades&) = delete;
    AdminAmistades& operator=(const AdminAmistades&) = delete;

    bool cargarUsuario(int id, const char* nombre);
    bool cargarAmistad(int primero, int segundo);
    bool cargarSolicitud(int emisor, int receptor);
    int buscarUsuario(int id) const;
    int buscarUsuario(const char* nombre) const;
    int getCantidadUsuarios() const;
    int getIdEnPosicion(int posicion) const;
    const char* getNombre(int id) const;
    bool sonAmigos(int primero, int segundo) const;
    bool haySolicitud(int emisor, int receptor) const;
    bool envioDeSolicitud(int emisor, int receptor);
    bool envioDeSolicitud(int emisor, const char* nombreReceptor);
    bool aceptarSolicitud(int receptor, int emisor);
    bool rechazarSolicitud(int receptor, int emisor);
    int contarAmigosComunes(int primero, int segundo) const;

    int sugerirAmigos(int usuario, int k, int* ids, int* comunes) const;
    void listarSolicitudes(int receptor, std::ostream& salida) const;
    void mostrarMenu(int usuario, std::istream& entrada, std::ostream& salida);
    void mostrarMenuSugerencias(int usuario, std::istream& entrada, std::ostream& salida);

    void reiniciarIteraciones() const;
    unsigned long long getIteraciones() const;
    std::size_t getMemoriaReservada() const;

private:
    int capacidad_;
    int cantidad_;
    int* ids_;
    char** nombres_;
    unsigned char* matriz_;
    std::size_t bytesNombres_;
    mutable unsigned long long iteraciones_;
    std::size_t celda(int fila, int columna) const;
    bool posicionesValidas(int primero, int segundo) const;
    bool nombresIguales(const char* primero, const char* segundo) const;
    bool registrarSolicitud(int emisor, int receptor);
    void ampliar();
};

#endif
