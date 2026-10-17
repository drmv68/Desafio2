#include "gestor_archivos.h"
#include <iostream>
#include <fstream>

using namespace std;

// ============================================================
// core/gestor_archivos.cpp
// Fase 1 - Funcionalidad I: Carga de Datos.
// leerPerfilCompleto(), actualizarLikes(), escribirNuevoMensaje()
// siguen pendientes para las siguientes funcionalidades.

static char* extraerCampo(const char* linea, int& inicio, char delimitador) {
    int fin = inicio;
    while (linea[fin] != delimitador && linea[fin] != '\0') {
        fin++;
    }

    int longitud = fin - inicio;
    char* campo = new char[longitud + 1];
    for (int i = 0; i < longitud; i++) {
        campo[i] = linea[inicio + i];
    }
    campo[longitud] = '\0';

    inicio = (linea[fin] == delimitador) ? fin + 1 : fin;
    return campo;
}

GestorArchivos::GestorArchivos() {
    arregloUsuariosLigeros = nullptr;
    totalUsuarios = 0;
}

GestorArchivos::~GestorArchivos() {
    delete[] arregloUsuariosLigeros;
}

// Funcion perteneciente a la clase GestorArchivos
bool GestorArchivos::cargarIndiceUsuarios(const char* rutaArchivo) {
    // Estado inicial: rutaArchivo = "estaticos_y_db/indice.txt". totalUsuarios = 0.

    ifstream archivoLectura(rutaArchivo);
    if (!archivoLectura.is_open()) {
        cout << "Error critico no se pudo abrir la base de datos ligera." << endl;
        return false;
    }

    // Leemos la primera linea que debe contener el numero total de usuarios registrados
    archivoLectura >> this->totalUsuarios;
    // [Debugger] El operador >> saca el primer numero del .txt.
    // Estado: totalUsuarios = 3 (ejemplo). OJO: el cursor del archivo queda justo
    // ANTES del salto de linea, >> nunca se come el '\n'.

    archivoLectura.ignore();
    // [Debugger] Sin esta linea, el primer getline() de abajo leeria una linea
    // VACIA (el '\n' que dejo el >> anterior) y todo el arreglo quedaria
    // desfasado un campo. Este es el bug mas comun de mezclar >> con getline().

    this->arregloUsuariosLigeros = new UsuarioLigero[this->totalUsuarios];
    // [Debugger] Se pide memoria dinamica (Heap).
    // Estado: arregloUsuariosLigeros apunta a un bloque de 3 casilleros, cada uno
    // ya inicializado por el constructor por defecto de UsuarioLigero (punteros en nullptr).

    const int TAM_MAX_LINEA = 256;
    char buffer[TAM_MAX_LINEA];

    int explorador = 0;

    // Ciclo para recorrer el archivo hasta llenarlo
    while (explorador < this->totalUsuarios) {

        archivoLectura.getline(buffer, TAM_MAX_LINEA);
        // Usamos getline (no >>) el nombre puede traer espacios

        int cursor = 0;
        char* idLeido = extraerCampo(buffer, cursor, ';');
        char* contrasenaLeida = extraerCampo(buffer, cursor, ';');
        char* nombreLeido = extraerCampo(buffer, cursor, ';');

        this->arregloUsuariosLigeros[explorador].setIdUsuario(idLeido);
        this->arregloUsuariosLigeros[explorador].setContrasena(contrasenaLeida);
        this->arregloUsuariosLigeros[explorador].setNombreUsuario(nombreLeido);

        delete[] idLeido;
        delete[] contrasenaLeida;
        delete[] nombreLeido;
        explorador++;

    }

    archivoLectura.close();

    cout << "indice cargado bien. total usuarios: " << this->totalUsuarios << endl;
    return true;
}

int GestorArchivos::getTotalUsuarios() const {
    return totalUsuarios;
}

const UsuarioLigero& GestorArchivos::getUsuarioLigero(int indice) const {
    return arregloUsuariosLigeros[indice];
}
