#ifndef GESTOR_MENSAJES_H
#define GESTOR_MENSAJES_H

#include "admin_amistades.h"
#include <cstddef>
#include <iosfwd>

// Tipos del UML necesarios para mensajeria. Se mantienen aqui para no editar
// el core pendiente de Daniel; al integrar pueden trasladarse a estructuras_base.
class Fecha {
public:
    Fecha(int dia = 1, int mes = 1, int anio = 2000);
    int getDia() const;
    int getMes() const;
    int getAnio() const;
    bool operator==(const Fecha& otra) const;
    bool operator<=(const Fecha& otra) const;
private:
    int valor_; // AAAAMMDD permite comparar sin cadenas ni copias de perfiles.
};

class Mensaje {
public:
    Mensaje(const Mensaje& otro); // Copia profunda del texto; no comparte propiedad.
    ~Mensaje();
    Mensaje& operator=(const Mensaje&) = delete;
    int getId() const;
    int getEmisor() const;
    int getReceptor() const;
    const Fecha& getFecha() const;
    const char* getTexto() const;
    bool estaLeido() const;
private:
    friend class GestorMensajes;
    Mensaje(int id, int emisor, int receptor, const Fecha& fecha,
            const char* texto, std::size_t bytes, bool leido);
    int id_, emisor_, receptor_;
    Fecha fecha_;
    char* texto_;
    std::size_t bytes_;
    bool leido_;
};

class GestorMensajes {
public:
    GestorMensajes();
    ~GestorMensajes();
    GestorMensajes(const GestorMensajes&) = delete;
    GestorMensajes& operator=(const GestorMensajes&) = delete;

    // GestorArchivos importa el historial, incluso si se carga fuera de orden.
    bool cargarMensaje(const AdminAmistades& red, int id, int emisor, int receptor,
                       const Fecha& fecha, const char* texto, bool leido);
    bool enviarMensaje(const AdminAmistades& red, int emisor,
                       const char* destinatario, const char* texto, const Fecha& fecha);
    int leerNuevos(const AdminAmistades& red, int receptor, std::ostream& salida);
    // No altera el estado leido; devuelve -1 para usuario/rango invalido.
    int buscarPorRangoDeFechas(const AdminAmistades& red, int receptor,
        const Fecha& inicio, const Fecha& fin, std::ostream& salida) const;
    void mostrarMenu(const AdminAmistades& red, int usuario, const Fecha& fechaActual,
                     std::istream& entrada, std::ostream& salida);
    int getCantidad() const;
    const Mensaje* getMensaje(int posicion) const; // Prestado, en orden cronologico.
    void reiniciarIteraciones() const;
    unsigned long long getIteraciones() const;
    std::size_t getMemoriaReservada() const; // Solo memoria poseida por este modulo.

private:
    Mensaje** mensajes_;
    int cantidad_, capacidad_, ultimoId_;
    std::size_t bytesMensajes_;
    mutable unsigned long long iteraciones_;
    bool validarTexto(const char* texto, std::size_t& bytes) const;
    void insertar(Mensaje* mensaje);
    void imprimir(const AdminAmistades& red, const Mensaje& mensaje, std::ostream& salida) const;
};

#endif // GESTOR_MENSAJES_H
