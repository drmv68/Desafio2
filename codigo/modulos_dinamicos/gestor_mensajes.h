#ifndef GESTOR_MENSAJES_H
#define GESTOR_MENSAJES_H

#include "admin_amistades.h"
#include "../core/estructuras_base.h"
#include <cstddef>
#include <iosfwd>

class Mensaje {
  public:
    Mensaje(const Mensaje &otro);
    ~Mensaje();
    Mensaje &operator=(const Mensaje &) = delete;
    int getId() const;
    int getEmisor() const;
    int getReceptor() const;
    const Fecha &getFecha() const;
    const char *getTexto() const;
    bool estaLeido() const;

  private:
    friend class GestorMensajes;
    Mensaje(int id, int emisor, int receptor, const Fecha &fecha, const char *texto,
            std::size_t bytes, bool leido);
    int id_, emisor_, receptor_;
    Fecha fecha_;
    char *texto_;
    std::size_t bytes_;
    bool leido_;
};

class GestorMensajes {
  public:
    GestorMensajes();
    ~GestorMensajes();
    GestorMensajes(const GestorMensajes &) = delete;
    GestorMensajes &operator=(const GestorMensajes &) = delete;

    bool cargarMensaje(const AdminAmistades &red, int id, int emisor, int receptor,
                       const Fecha &fecha, const char *texto, bool leido);
    bool enviarMensaje(const AdminAmistades &red, int emisor, const char *destinatario,
                       const char *texto, const Fecha &fecha);
    int leerNuevos(const AdminAmistades &red, int receptor, std::ostream &salida);
    int buscarPorRangoDeFechas(const AdminAmistades &red, int receptor, const Fecha &inicio,
                               const Fecha &fin, std::ostream &salida) const;
    void mostrarMenu(const AdminAmistades &red, int usuario, const Fecha &fechaActual,
                     std::istream &entrada, std::ostream &salida,
                     void (*alTerminar)(void *) = nullptr, void *contexto = nullptr);
    int getCantidad() const;
    const Mensaje *getMensaje(int posicion) const;
    bool hayCambios() const;
    void confirmarGuardado();
    void reiniciarIteraciones() const;
    unsigned long long getIteraciones() const;
    std::size_t getMemoriaReservada() const;

  private:
    bool cambiosPendientes_;
    Mensaje **mensajes_;
    int cantidad_, capacidad_, ultimoId_;
    std::size_t bytesMensajes_;
    bool validarTexto(const char *texto, std::size_t &bytes) const;
    void insertar(Mensaje *mensaje);
    void imprimir(const AdminAmistades &red, const Mensaje &mensaje, std::ostream &salida) const;
};

#endif
