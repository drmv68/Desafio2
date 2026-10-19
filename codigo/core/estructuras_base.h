#ifndef ESTRUCTURAS_BASE_H
#define ESTRUCTURAS_BASE_H

// Core structures: lightweight user and message skeletons.

class UsuarioLigero {
private:
    // Todo char*: nada de std::string, porque el desafio exige
    // cero contenedores de la STL y memoria dinamica de creacion
    // propia (seccion "Requisitos de la entrega" del enunciado).
    char* idUsuario;
    char* contrasena;
    char* nombreUsuario;

public:
    UsuarioLigero();                             // constructor por defecto
    UsuarioLigero(const UsuarioLigero& otro);     // constructor de copia
    ~UsuarioLigero();                             // destructor: libera los 3 char*

    // TODO (cuando lo necesitemos): operator= propio. Por ahora
    // nunca hacemos "objetoA = objetoB" con UsuarioLigero, solo
    // usamos los setters campo por campo, asi que no hace falta
    // todavia (pero con constructor de copia + destructor propios,
    // la "regla de los 3" dice que tarde o temprano hay que ponerlo).

    void setIdUsuario(const char* valor);
    void setContrasena(const char* valor);
    void setNombreUsuario(const char* valor);

    const char* getIdUsuario() const;
    const char* getContrasena() const;
    const char* getNombreUsuario() const;
};

class EstructuraMensaje {
    // Atributos macro: id_remitente, fecha, ruta_archivo
    // Futura sobrecarga de operadores para comparar fechas ira aqui.
};

// ============================================================
// Agregado para la Funcionalidad IV (Visitar perfil de un amigo).
// Una foto con su cantidad de likes y si es publica o solo para
// amigos. Vive aqui (no en gestor_archivos.h) porque, igual que
// UsuarioLigero, es una estructura de datos reutilizable, no
// logica de un modulo dinamico en particular.
// ============================================================
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

// Agrupa todo lo que VisorPerfiles necesita de un usuario: la
// carpeta fisica donde estan sus fotos y el catalogo de fotos
// (cada una con su nombre, sus likes y si es publica). La arma
// GestorArchivos::leerPerfilCompleto() leyendo el .txt del perfil.
class PerfilCompleto {
private:
    char* rutaCarpetaDeFotos;
    FotoConLikes* fotos; // arreglo dinamico, tamano = cantidadDeFotos
    int cantidadDeFotos;

public:
    PerfilCompleto();
    PerfilCompleto(const PerfilCompleto& otro);
    ~PerfilCompleto();

    void setRutaCarpetaDeFotos(const char* valor);
    void inicializarFotos(int cantidad); // reserva el arreglo dinamico de fotos

    const char* getRutaCarpetaDeFotos() const;
    int getCantidadDeFotos() const;
    FotoConLikes& getFotoEnPosicion(int posicion); // sin const: VisorPerfiles edita el like
};

#endif // ESTRUCTURAS_BASE_H
