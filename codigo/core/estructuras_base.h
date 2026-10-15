#ifndef ESTRUCTURAS_BASE_H
#define ESTRUCTURAS_BASE_H

// ============================================================
// core/estructuras_base.h
// Modulo 0.1 del analisis: "Statics, estructuras y archivos".
// Aqui viven los objetos ligeros y esqueletos que se reutilizan
// entre modulos dinamicos (busquedas eficientes por nombre/ID,
// representacion basica de un mensaje para listas por fecha, etc).
//
// FASE 1 (en aterrizaje): UsuarioLigero ya tiene atributos y
// metodos reales porque gestor_archivos.cpp la necesita para
// cargar el indice de usuarios. EstructuraMensaje sigue en
// fase macro, se aterriza cuando lleguemos al modulo 6.
// ============================================================

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
