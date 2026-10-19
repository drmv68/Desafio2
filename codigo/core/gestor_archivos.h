#ifndef GESTOR_ARCHIVOS_H
#define GESTOR_ARCHIVOS_H
#include <cstddef>
#include "estructuras_base.h"
class AdminAmistades;
class GestorMensajes;

class GestorArchivos {
    UsuarioLigero *arregloUsuariosLigeros;
    int totalUsuarios;
    char rutaBase[1024];
    bool construirRutaPerfil(const char *id, char *destino, std::size_t capacidad) const;

  public:
    GestorArchivos();
    ~GestorArchivos();
    GestorArchivos(const GestorArchivos &) = delete;
    GestorArchivos &operator=(const GestorArchivos &) = delete;
    bool cargarIndiceUsuarios(const char *rutaArchivo);
    bool construirRuta(const char *archivo, char *destino, std::size_t capacidad) const;
    int getTotalUsuarios() const;
    const UsuarioLigero &getUsuarioLigero(int indice) const;
    bool leerRutaFotoPerfil(const char *id, char *destino, std::size_t capacidad) const;
    bool leerPerfilCompleto(const char *idUsuario, PerfilCompleto &destino) const;
    bool guardarCambioDeLike(const char *idUsuario, const char *foto, int likes,
                             const char *album = nullptr) const;
    bool cargarAmistades(const char *rutaArchivo, AdminAmistades &red) const;
    bool cargarSolicitudes(const char *rutaArchivo, AdminAmistades &red) const;
    bool cargarMensajes(const char *rutaArchivo, const AdminAmistades &red,
                        GestorMensajes &mensajes) const;
    bool guardarEstado(AdminAmistades &red, GestorMensajes &mensajes) const;
    void reiniciarIteraciones() const;
    unsigned long long getIteraciones() const;
    std::size_t getMemoriaReservada() const;
};
#endif
