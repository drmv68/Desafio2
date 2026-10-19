#ifndef ESTRUCTURAS_BASE_H
#define ESTRUCTURAS_BASE_H
#include <cstddef>

class Fecha {
    int valor_;

  public:
    Fecha(int dia = 1, int mes = 1, int anio = 2000);
    static Fecha actual();
    int getDia() const;
    int getMes() const;
    int getAnio() const;
    bool operator==(const Fecha &otra) const;
    bool operator<=(const Fecha &otra) const;
};

class UsuarioLigero {
    char *idUsuario;
    char *contrasena;
    char *nombreUsuario;

  public:
    UsuarioLigero();
    UsuarioLigero(const UsuarioLigero &otro);
    UsuarioLigero &operator=(const UsuarioLigero &otro);
    ~UsuarioLigero();
    void setIdUsuario(const char *valor);
    void setContrasena(const char *valor);
    void setNombreUsuario(const char *valor);
    const char *getIdUsuario() const;
    const char *getContrasena() const;
    const char *getNombreUsuario() const;
    std::size_t getMemoriaReservada() const;
};

class FotoConLikes {
    char *nombreDeArchivo;
    int cantidadDeLikes;
    int indiceAlbum;
    bool likeEnSesion;
    Fecha fechaCaptura;
    std::size_t tamano;

  public:
    FotoConLikes();
    FotoConLikes(const FotoConLikes &otra);
    FotoConLikes &operator=(const FotoConLikes &otra);
    ~FotoConLikes();
    void setNombreDeArchivo(const char *valor);
    void setCantidadDeLikes(int valor);
    void setIndiceAlbum(int valor);
    void setFechaCaptura(const Fecha &valor);
    void setTamano(std::size_t valor);
    void setLikeEnSesion(bool valor);
    const char *getNombreDeArchivo() const;
    int getCantidadDeLikes() const;
    int getIndiceAlbum() const;
    const Fecha &getFechaCaptura() const;
    std::size_t getTamano() const;
    bool getLikeEnSesion() const;
    std::size_t getMemoriaReservada() const;
};

class Album {
    char *nombre;
    int privacidad; // 0 amigos, 1 publico, 2 privado (solo propietario).
  public:
    Album();
    Album(const Album &otro);
    Album &operator=(const Album &otro);
    ~Album();
    void configurar(const char *nombreNuevo, int privacidadNueva);
    const char *getNombre() const;
    int getPrivacidad() const;
    std::size_t getMemoriaReservada() const;
};

class PerfilCompleto {
    char *rutaCarpetaDeFotos;
    FotoConLikes *fotos;
    int cantidadDeFotos;
    Album *albumes;
    int cantidadDeAlbumes;
    int capacidadDeAlbumes;
    char *correo;
    char *ciudad;
    char *pais;
    Fecha nacimiento;
    Fecha inscripcion;

  public:
    PerfilCompleto();
    PerfilCompleto(const PerfilCompleto &otro);
    PerfilCompleto &operator=(const PerfilCompleto &otro);
    ~PerfilCompleto();
    void setRutaCarpetaDeFotos(const char *valor);
    void inicializarFotos(int cantidad);
    int incorporarAlbum(const char *nombre, int privacidad);
    void setDatosUsuario(const char *correo, const char *ciudad, const char *pais,
                         const Fecha &nacimiento, const Fecha &inscripcion);
    const char *getCorreo() const;
    const char *getCiudad() const;
    const char *getPais() const;
    const Fecha &getNacimiento() const;
    const Fecha &getInscripcion() const;
    const char *getRutaCarpetaDeFotos() const;
    int getCantidadDeFotos() const;
    int getCantidadDeAlbumes() const;
    FotoConLikes &getFotoEnPosicion(int posicion);
    const FotoConLikes &getFotoEnPosicion(int posicion) const;
    const Album &getAlbum(int posicion) const;
    std::size_t getMemoriaReservada() const;
};
#endif
