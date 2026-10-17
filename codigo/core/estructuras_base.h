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

#endif // ESTRUCTURAS_BASE_H
