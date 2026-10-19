#include "autenticacion.h"
#include <iostream>

using namespace std;

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

static int convertirTextoANumero(const char* texto) {
    int numero = 0;
    int posicion = 0;
    while (texto[posicion] != '\0') {
        numero = numero * 10 + (texto[posicion] - '0');
        posicion++;
    }
    return numero;
}

Autenticacion::Autenticacion() {
    idDelUsuarioActivo = nullptr;
    iteracionesDadas = 0;
}

Autenticacion::~Autenticacion() {
    delete[] idDelUsuarioActivo;
}

bool Autenticacion::iniciarSesion(const GestorArchivos& baseDatos) {
    const int TAM_MAX_TEXTO = 100;
    char idQueEscribioElUsuario[TAM_MAX_TEXTO];
    char contrasenaQueEscribioElUsuario[TAM_MAX_TEXTO];

    cout << "Id de usuario: ";
    cin.getline(idQueEscribioElUsuario, TAM_MAX_TEXTO);

    cout << "Contrasena: ";
    cin.getline(contrasenaQueEscribioElUsuario, TAM_MAX_TEXTO);

    int posicionEncontrada = -1;
    int posicionActual = 0;
    while (posicionActual < baseDatos.getTotalUsuarios() && posicionEncontrada == -1) {
        iteracionesDadas++;

        const UsuarioLigero& usuarioActual = baseDatos.getUsuarioLigero(posicionActual);
        if (textosIguales(usuarioActual.getIdUsuario(), idQueEscribioElUsuario)) {
            posicionEncontrada = posicionActual;
        }

        posicionActual++;
    }

    if (posicionEncontrada == -1) {
        cout << "Ese id no existe." << endl;
        return false;
    }

    const UsuarioLigero& usuarioEncontrado = baseDatos.getUsuarioLigero(posicionEncontrada);
    bool contrasenaCorrecta = textosIguales(usuarioEncontrado.getContrasena(), contrasenaQueEscribioElUsuario);

    if (!contrasenaCorrecta) {
        cout << "Contrasena incorrecta." << endl;
        return false;
    }

    delete[] idDelUsuarioActivo;
    idDelUsuarioActivo = nullptr;

    int longitudDelId = 0;
    while (usuarioEncontrado.getIdUsuario()[longitudDelId] != '\0') {
        longitudDelId++;
    }

    idDelUsuarioActivo = new char[longitudDelId + 1];
    int posicionDeCopia = 0;
    while (posicionDeCopia < longitudDelId) {
        idDelUsuarioActivo[posicionDeCopia] = usuarioEncontrado.getIdUsuario()[posicionDeCopia];
        posicionDeCopia++;
    }
    idDelUsuarioActivo[longitudDelId] = '\0';

    cout << "Bienvenido, " << usuarioEncontrado.getNombreUsuario() << "." << endl;
    return true;
}

const char* Autenticacion::getIdDelUsuarioActivo() const {
    return idDelUsuarioActivo;
}

void Autenticacion::mostrarMejoresAmigos(const AdminAmistades& redDeAmistades) const {
    if (idDelUsuarioActivo == nullptr) {
        cout << "Todavia no ha iniciado sesion nadie." << endl;
        return;
    }

    int idPropioComoNumero = convertirTextoANumero(idDelUsuarioActivo);

    int mejorAmigoEncontrado = -1;
    int mayorCantidadDeAmigosComunes = -1;

    int posicionActual = 0;
    while (posicionActual < redDeAmistades.getCantidadUsuarios()) {
        iteracionesDadas++;

        int idCandidato = redDeAmistades.getIdEnPosicion(posicionActual);

        if (redDeAmistades.sonAmigos(idPropioComoNumero, idCandidato)) {
            int cantidadDeAmigosComunes = redDeAmistades.contarAmigosComunes(idPropioComoNumero, idCandidato);
            if (cantidadDeAmigosComunes > mayorCantidadDeAmigosComunes) {
                mayorCantidadDeAmigosComunes = cantidadDeAmigosComunes;
                mejorAmigoEncontrado = idCandidato;
            }
        }

        posicionActual++;
    }

    if (mejorAmigoEncontrado != -1) {
        cout << "Tu amigo con mas prioridad es: " << redDeAmistades.getNombre(mejorAmigoEncontrado)
             << " (comparten " << mayorCantidadDeAmigosComunes << " amigos en comun)." << endl;
    } else {
        cout << "Todavia no tienes amigos confirmados." << endl;
    }

    const int CANTIDAD_CANDIDATOS = 3;
    int idsCandidatos[CANTIDAD_CANDIDATOS];
    int comunesCandidatos[CANTIDAD_CANDIDATOS];
    int totalCandidatos = redDeAmistades.sugerirAmigos(idPropioComoNumero, CANTIDAD_CANDIDATOS, idsCandidatos, comunesCandidatos);

    if (totalCandidatos > 0) {
        cout << "\nCandidatos sugeridos para conectar:" << endl;
        int i = 0;
        while (i < totalCandidatos) {
            cout << "  " << (i + 1) << ". " << redDeAmistades.getNombre(idsCandidatos[i])
                 << " (" << comunesCandidatos[i] << " amigos en comun)" << endl;
            i++;
        }
    }
}

void Autenticacion::reiniciarIteraciones() const {
    iteracionesDadas = 0;
}

unsigned long long Autenticacion::getIteraciones() const {
    return iteracionesDadas;
}

std::size_t Autenticacion::getMemoriaReservada() const {
    return sizeof(*this);
}
