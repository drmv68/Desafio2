#ifndef ESTRUCTURAS_BASE_H
#define ESTRUCTURAS_BASE_H

class UsuarioLigero {
private:
    char* idUsuario;
    char* contrasena;
    char* nombreUsuario;

public:
    UsuarioLigero();
    UsuarioLigero(const UsuarioLigero& otro);
    ~UsuarioLigero();

    void setIdUsuario(const char* valor);
    void setContrasena(const char* valor);
    void setNombreUsuario(const char* valor);

    const char* getIdUsuario() const;
    const char* getContrasena() const;
    const char* getNombreUsuario() const;
};

class FotoConLikes {
private:
    char* nombreDeArchivo;
    int cantidadDeLikes;
    bool esPublica;

public:
    FotoConLikes();
    FotoConLikes(const FotoConLikes& otra);
    ~FotoConLikes();

    void setNombreDeArchivo(const char* valor);
    void setCantidadDeLikes(int valor);
    void setEsPublica(bool valor);

    const char* getNombreDeArchivo() const;
    int getCantidadDeLikes() const;
    bool getEsPublica() const;
};

class PerfilCompleto {
private:
    char* rutaCarpetaDeFotos;
    FotoConLikes* fotos;
    int cantidadDeFotos;

public:
    PerfilCompleto();
    PerfilCompleto(const PerfilCompleto& otro);
    ~PerfilCompleto();

    void setRutaCarpetaDeFotos(const char* valor);
    void inicializarFotos(int cantidad);

    const char* getRutaCarpetaDeFotos() const;
    int getCantidadDeFotos() const;
    FotoConLikes& getFotoEnPosicion(int posicion);
};

#endif
