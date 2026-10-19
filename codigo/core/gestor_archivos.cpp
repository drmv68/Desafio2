#include "gestor_archivos.h"
#include <iostream>
#include <fstream>
#include "../modulos_dinamicos/admin_amistades.h"
#include "../modulos_dinamicos/gestor_mensajes.h"

using namespace std;

static int convertirTextoANumero(const char* texto) {
    int numero = 0;
    int posicion = 0;
    while (texto[posicion] != '\0') {
        numero = numero * 10 + (texto[posicion] - '0');
        posicion++;
    }
    return numero;
}

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

static void quitarRetornoDeCarroSiExiste(char* texto) {
    int longitud = 0;
    while (texto[longitud] != '\0') {
        longitud++;
    }
    if (longitud > 0 && texto[longitud - 1] == '\r') {
        texto[longitud - 1] = '\0';
    }
}

static bool textosIguales(const char* primero, const char* segundo) {
    int posicion = 0;
    while (primero[posicion] != '\0' && segundo[posicion] != '\0') {
        if (primero[posicion] != segundo[posicion]) {
            return false;
        }
        posicion++;
    }
    return primero[posicion] == segundo[posicion];
}

static void construirRutaDePerfil(const char* idUsuario, char* rutaDestino, int tamanoMaximo) {
    const char* prefijo = "estaticos_y_db/perfiles/";
    const char* sufijo = ".txt";
    int posicionDestino = 0;

    int posicionPrefijo = 0;
    while (prefijo[posicionPrefijo] != '\0' && posicionDestino < tamanoMaximo - 1) {
        rutaDestino[posicionDestino] = prefijo[posicionPrefijo];
        posicionDestino++;
        posicionPrefijo++;
    }

    int posicionId = 0;
    while (idUsuario[posicionId] != '\0' && posicionDestino < tamanoMaximo - 1) {
        rutaDestino[posicionDestino] = idUsuario[posicionId];
        posicionDestino++;
        posicionId++;
    }

    int posicionSufijo = 0;
    while (sufijo[posicionSufijo] != '\0' && posicionDestino < tamanoMaximo - 1) {
        rutaDestino[posicionDestino] = sufijo[posicionSufijo];
        posicionDestino++;
        posicionSufijo++;
    }

    rutaDestino[posicionDestino] = '\0';
}

GestorArchivos::GestorArchivos() {
    arregloUsuariosLigeros = nullptr;
    totalUsuarios = 0;
    iteracionesDadas = 0;
}

GestorArchivos::~GestorArchivos() {
    delete[] arregloUsuariosLigeros;
}

bool GestorArchivos::cargarIndiceUsuarios(const char* rutaArchivo) {
    ifstream archivoLectura(rutaArchivo);
    if (!archivoLectura.is_open()) {
        cout << "Error critico no se pudo abrir la base de datos ligera." << endl;
        return false;
    }

    archivoLectura >> this->totalUsuarios;
    archivoLectura.ignore();

    this->arregloUsuariosLigeros = new UsuarioLigero[this->totalUsuarios];

    const int TAM_MAX_LINEA = 256;
    char buffer[TAM_MAX_LINEA];

    int explorador = 0;
    while (explorador < this->totalUsuarios) {
        iteracionesDadas++;

        archivoLectura.getline(buffer, TAM_MAX_LINEA);
        quitarRetornoDeCarroSiExiste(buffer);

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

bool GestorArchivos::leerPerfilCompleto(const char* idUsuario, PerfilCompleto& perfilDestino) const {
    iteracionesDadas++;

    const int TAM_MAX_RUTA = 256;
    char rutaDelPerfil[TAM_MAX_RUTA];
    construirRutaDePerfil(idUsuario, rutaDelPerfil, TAM_MAX_RUTA);

    ifstream archivoLectura(rutaDelPerfil);
    if (!archivoLectura.is_open()) {
        cout << "Error: no existe el archivo de perfil para el usuario " << idUsuario << endl;
        return false;
    }

    const int TAM_MAX_LINEA = 256;
    char buffer[TAM_MAX_LINEA];

    archivoLectura.getline(buffer, TAM_MAX_LINEA);
    quitarRetornoDeCarroSiExiste(buffer);
    perfilDestino.setRutaCarpetaDeFotos(buffer);

    int cantidadDeFotos = 0;
    archivoLectura >> cantidadDeFotos;
    archivoLectura.ignore();

    perfilDestino.inicializarFotos(cantidadDeFotos);

    int posicionActual = 0;
    while (posicionActual < cantidadDeFotos) {
        iteracionesDadas++;

        archivoLectura.getline(buffer, TAM_MAX_LINEA);
        quitarRetornoDeCarroSiExiste(buffer);

        int cursor = 0;
        char* nombreLeido = extraerCampo(buffer, cursor, ';');
        char* likesLeidosTexto = extraerCampo(buffer, cursor, ';');
        char* esPublicaTexto = extraerCampo(buffer, cursor, ';');

        int likesLeidos = 0;
        int posicionDigito = 0;
        while (likesLeidosTexto[posicionDigito] != '\0') {
            likesLeidos = likesLeidos * 10 + (likesLeidosTexto[posicionDigito] - '0');
            posicionDigito++;
        }

        bool esPublicaLeida = (esPublicaTexto[0] == '1');

        FotoConLikes& fotoActual = perfilDestino.getFotoEnPosicion(posicionActual);
        fotoActual.setNombreDeArchivo(nombreLeido);
        fotoActual.setCantidadDeLikes(likesLeidos);
        fotoActual.setEsPublica(esPublicaLeida);

        delete[] nombreLeido;
        delete[] likesLeidosTexto;
        delete[] esPublicaTexto;

        posicionActual = posicionActual + 1;
    }

    archivoLectura.close();
    return true;
}

bool GestorArchivos::guardarCambioDeLike(const char* idUsuario, const char* nombreDeArchivo,
                                          int nuevaCantidadDeLikes) const {
    iteracionesDadas++;

    PerfilCompleto perfilActual;
    bool seLeyoBien = leerPerfilCompleto(idUsuario, perfilActual);
    if (!seLeyoBien) {
        return false;
    }

    bool seEncontroLaFoto = false;
    int posicionActual = 0;
    while (posicionActual < perfilActual.getCantidadDeFotos()) {
        iteracionesDadas++;

        FotoConLikes& fotoActual = perfilActual.getFotoEnPosicion(posicionActual);
        if (textosIguales(fotoActual.getNombreDeArchivo(), nombreDeArchivo)) {
            fotoActual.setCantidadDeLikes(nuevaCantidadDeLikes);
            seEncontroLaFoto = true;
        }

        posicionActual = posicionActual + 1;
    }

    if (!seEncontroLaFoto) {
        cout << "Error: esa foto no existe en el perfil del usuario " << idUsuario << endl;
        return false;
    }

    const int TAM_MAX_RUTA = 256;
    char rutaDelPerfil[TAM_MAX_RUTA];
    construirRutaDePerfil(idUsuario, rutaDelPerfil, TAM_MAX_RUTA);

    ofstream archivoEscritura(rutaDelPerfil);
    if (!archivoEscritura.is_open()) {
        cout << "Error: no se pudo volver a abrir el archivo de perfil para guardar el like." << endl;
        return false;
    }

    archivoEscritura << perfilActual.getRutaCarpetaDeFotos() << endl;
    archivoEscritura << perfilActual.getCantidadDeFotos() << endl;

    posicionActual = 0;
    while (posicionActual < perfilActual.getCantidadDeFotos()) {
        FotoConLikes& fotoActual = perfilActual.getFotoEnPosicion(posicionActual);
        archivoEscritura << fotoActual.getNombreDeArchivo() << ";"
                          << fotoActual.getCantidadDeLikes() << ";"
                          << (fotoActual.getEsPublica() ? 1 : 0) << endl;
        posicionActual = posicionActual + 1;
    }

    archivoEscritura.close();
    return true;
}

bool GestorArchivos::cargarAmistades(const char* rutaArchivo, AdminAmistades& red) const {
    ifstream archivoLectura(rutaArchivo);
    if (!archivoLectura.is_open()) {
        return false;
    }

    int totalAmistades = 0;
    archivoLectura >> totalAmistades;
    archivoLectura.ignore();

    const int TAM_MAX_LINEA = 256;
    char buffer[TAM_MAX_LINEA];

    int i = 0;
    while (i < totalAmistades && archivoLectura.getline(buffer, TAM_MAX_LINEA)) {
        iteracionesDadas++;
        quitarRetornoDeCarroSiExiste(buffer);

        int cursor = 0;
        char* id1Leido = extraerCampo(buffer, cursor, ';');
        char* id2Leido = extraerCampo(buffer, cursor, ';');

        int id1 = convertirTextoANumero(id1Leido);
        int id2 = convertirTextoANumero(id2Leido);

        red.cargarAmistad(id1, id2);

        delete[] id1Leido;
        delete[] id2Leido;
        i++;
    }

    archivoLectura.close();
    return true;
}

bool GestorArchivos::cargarSolicitudes(const char* rutaArchivo, AdminAmistades& red) const {
    ifstream archivoLectura(rutaArchivo);
    if (!archivoLectura.is_open()) {
        return false;
    }

    int totalSolicitudes = 0;
    archivoLectura >> totalSolicitudes;
    archivoLectura.ignore();

    const int TAM_MAX_LINEA = 256;
    char buffer[TAM_MAX_LINEA];

    int i = 0;
    while (i < totalSolicitudes && archivoLectura.getline(buffer, TAM_MAX_LINEA)) {
        iteracionesDadas++;
        quitarRetornoDeCarroSiExiste(buffer);

        int cursor = 0;
        char* idEmisorLeido = extraerCampo(buffer, cursor, ';');
        char* idReceptorLeido = extraerCampo(buffer, cursor, ';');

        int emisor = convertirTextoANumero(idEmisorLeido);
        int receptor = convertirTextoANumero(idReceptorLeido);

        red.cargarSolicitud(emisor, receptor);

        delete[] idEmisorLeido;
        delete[] idReceptorLeido;
        i++;
    }

    archivoLectura.close();
    return true;
}

bool GestorArchivos::cargarMensajes(const char* rutaArchivo, const AdminAmistades& red,
                                    GestorMensajes& moduloMensajes) const {
    ifstream archivoLectura(rutaArchivo);
    if (!archivoLectura.is_open()) {
        return false;
    }

    int totalMensajes = 0;
    archivoLectura >> totalMensajes;
    archivoLectura.ignore();

    const int TAM_MAX_LINEA = 2048;
    char buffer[TAM_MAX_LINEA];

    int i = 0;
    while (i < totalMensajes && archivoLectura.getline(buffer, TAM_MAX_LINEA)) {
        iteracionesDadas++;
        quitarRetornoDeCarroSiExiste(buffer);

        int cursor = 0;
        char* idLeido = extraerCampo(buffer, cursor, ';');
        char* emisorLeido = extraerCampo(buffer, cursor, ';');
        char* receptorLeido = extraerCampo(buffer, cursor, ';');
        char* diaLeido = extraerCampo(buffer, cursor, ';');
        char* mesLeido = extraerCampo(buffer, cursor, ';');
        char* anioLeido = extraerCampo(buffer, cursor, ';');
        char* textoLeido = extraerCampo(buffer, cursor, ';');
        char* leidoTexto = extraerCampo(buffer, cursor, ';');

        int id = convertirTextoANumero(idLeido);
        int emisor = convertirTextoANumero(emisorLeido);
        int receptor = convertirTextoANumero(receptorLeido);
        int dia = convertirTextoANumero(diaLeido);
        int mes = convertirTextoANumero(mesLeido);
        int anio = convertirTextoANumero(anioLeido);
        bool estaLeido = (leidoTexto[0] == '1');

        moduloMensajes.cargarMensaje(red, id, emisor, receptor, Fecha(dia, mes, anio), textoLeido, estaLeido);

        delete[] idLeido;
        delete[] emisorLeido;
        delete[] receptorLeido;
        delete[] diaLeido;
        delete[] mesLeido;
        delete[] anioLeido;
        delete[] textoLeido;
        delete[] leidoTexto;

        i++;
    }

    archivoLectura.close();
    return true;
}

void GestorArchivos::reiniciarIteraciones() const {
    iteracionesDadas = 0;
}

unsigned long long GestorArchivos::getIteraciones() const {
    return iteracionesDadas;
}

std::size_t GestorArchivos::getMemoriaReservada() const {
    return sizeof(*this) + static_cast<std::size_t>(totalUsuarios) * sizeof(UsuarioLigero);
}
