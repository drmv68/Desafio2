#include "gestor_archivos.h"
#include <iostream>
#include <fstream>

using namespace std;

// ============================================================
// core/gestor_archivos.cpp
// Fase 1 - Funcionalidad I: Carga de Datos.
// leerPerfilCompleto(), actualizarLikes(), escribirNuevoMensaje()
// siguen pendientes para las siguientes funcionalidades.
// ============================================================

// Funcion auxiliar INTERNA de este modulo (no esta en el .h, por
// lo tanto nadie fuera de este .cpp puede usarla -> cumple la
// regla de "ninguna auxiliar de un modulo se usa en otro").
//
// Recorre 'linea' desde 'inicio' hasta toparse con 'delimitador'
// o con el final del texto, reserva memoria EXACTA para ese
// pedazo y lo copia. Deja 'inicio' apuntando al siguiente campo.
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
    // [Debugger] Constructor: todavia no hay nada cargado del disco.
}

GestorArchivos::~GestorArchivos() {
    delete[] arregloUsuariosLigeros;
    // [Debugger] Libera TODO el arreglo de un solo golpe. C++ ya se
    // encarga de llamar, uno por uno, al destructor de cada
    // UsuarioLigero del arreglo antes de liberar el bloque completo.
}

// Funcion perteneciente a la clase GestorArchivos
bool GestorArchivos::cargarIndiceUsuarios(const char* rutaArchivo) {

    // [Debugger] Inicio de ejecucion de cargarIndiceUsuarios().
    // Estado inicial: rutaArchivo = "estaticos_y_db/indice.txt". totalUsuarios = 0.

    ifstream archivoLectura(rutaArchivo);
    // [Debugger] Se crea el objeto ifstream para abrir el archivo en modo texto.
    // Estado: archivoLectura esta intentando conectar con el disco duro.

    // Verificacion de seguridad
    if (!archivoLectura.is_open()) {
        cout << "Error critico: No se pudo abrir la base de datos ligera." << endl;
        // [Debugger] Si (!archivoLectura.is_open()) es TRUE, el archivo no existe o la ruta esta mal.
        return false;
    }

    // [Debugger] El archivo se abrio con exito. Condicion FALSE, saltamos el if.

    // Leemos la primera linea que debe contener el numero total de usuarios registrados
    archivoLectura >> this->totalUsuarios;
    // [Debugger] El operador >> saca el primer numero del .txt.
    // Estado: totalUsuarios = 3 (ejemplo). OJO: el cursor del archivo queda justo
    // ANTES del salto de linea, >> nunca se come el '\n'.

    archivoLectura.ignore();
    // [Debugger] Sin esta linea, el primer getline() de abajo leeria una linea
    // VACIA (el '\n' que dejo el >> anterior) y todo el arreglo quedaria
    // desfasado un campo. Este es el bug mas comun de mezclar >> con getline().

    // Uso de Memoria Dinamica (Regla estricta del desafio)
    this->arregloUsuariosLigeros = new UsuarioLigero[this->totalUsuarios];
    // [Debugger] Se pide memoria dinamica (Heap).
    // Estado: arregloUsuariosLigeros apunta a un bloque de 3 casilleros, cada uno
    // ya inicializado por el constructor por defecto de UsuarioLigero (punteros en nullptr).

    const int TAM_MAX_LINEA = 256;
    char buffer[TAM_MAX_LINEA];
    // [Debugger] Buffer temporal en la pila (stack). Se reescribe en cada vuelta
    // del ciclo; lo que sobrevive es lo que copiamos al Heap con extraerCampo().

    int explorador = 0;
    // [Debugger] Variable iteradora creada. Estado: explorador = 0.

    // Ciclo para recorrer el archivo hasta llenarlo
    while (explorador < this->totalUsuarios) {

        // [Debugger] Evaluando: ?0 < 3? (TRUE). Entra al ciclo.

        archivoLectura.getline(buffer, TAM_MAX_LINEA);
        // [Debugger] Lee la linea completa, ej: "1001;pass123;daniel mosquera".
        // Usamos getline (no >>) justamente porque el nombre puede traer espacios.

        int cursor = 0;
        char* idLeido = extraerCampo(buffer, cursor, ';');
        char* contrasenaLeida = extraerCampo(buffer, cursor, ';');
        char* nombreLeido = extraerCampo(buffer, cursor, ';');
        // [Debugger] La linea se parte en 3 pedazos usando ';' como separador.
        // Cada pedazo ya vive en su propio new char[] (memoria del Heap).

        this->arregloUsuariosLigeros[explorador].setIdUsuario(idLeido);
        this->arregloUsuariosLigeros[explorador].setContrasena(contrasenaLeida);
        this->arregloUsuariosLigeros[explorador].setNombreUsuario(nombreLeido);
        // [Debugger] Los setters hacen SU PROPIA copia interna (ver estructuras_base.cpp).

        delete[] idLeido;
        delete[] contrasenaLeida;
        delete[] nombreLeido;
        // [Debugger] Liberamos los temporales: el dato real ya quedo copiado
        // dentro del UsuarioLigero, mantener estos tres seria memoria duplicada.

        explorador++;
        // [Debugger] Suma 1 al contador. El ciclo se repite hasta llenar el arreglo.
    }

    archivoLectura.close();
    // [Debugger] Se cierra la conexion con el disco duro para liberar recursos.
    // Estado: arregloUsuariosLigeros esta 100% lleno en la memoria RAM.

    cout << "Indice cargado exitosamente. Total usuarios: " << this->totalUsuarios << endl;
    return true;
    // [Debugger] Fin exitoso. Se destruyen variables locales (buffer, explorador,
    // cursor) pero el arreglo dinamico sigue vivo en el Heap, listo para que
    // Autenticacion lo use.
}

int GestorArchivos::getTotalUsuarios() const {
    return totalUsuarios;
}

const UsuarioLigero& GestorArchivos::getUsuarioLigero(int indice) const {
    return arregloUsuariosLigeros[indice];
    // [Debugger] Se retorna por REFERENCIA (const UsuarioLigero&), no por copia:
    // evita invocar el constructor de copia innecesariamente cada vez que alguien
    // solo quiere consultar un usuario (ver nota de eficiencia del enunciado).
}
