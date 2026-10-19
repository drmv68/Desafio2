#include "menu.h"
#include <iostream>
#include <limits>

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

Menu::Menu(GestorArchivos& baseDatosInicial, Autenticacion& accesoInicial,
           AdminAmistades& amistadesInicial, VisorPerfiles& perfilesInicial,
           GestorMensajes& mensajesInicial, MonitorRecursos& monitorInicial)
    : baseDatos(baseDatosInicial), moduloAcceso(accesoInicial), moduloAmistades(amistadesInicial),
      moduloPerfiles(perfilesInicial), moduloMensajes(mensajesInicial), monitor(monitorInicial) {
}

void Menu::cargarUsuariosEnRedDeAmistades() {
    int posicionDeCarga = 0;
    while (posicionDeCarga < baseDatos.getTotalUsuarios()) {
        const UsuarioLigero& usuarioActual = baseDatos.getUsuarioLigero(posicionDeCarga);
        int idComoNumero = convertirTextoANumero(usuarioActual.getIdUsuario());
        moduloAmistades.cargarUsuario(idComoNumero, usuarioActual.getNombreUsuario());
        posicionDeCarga++;
    }
}

void Menu::mostrarReporteDeTodos() {
    const int CANTIDAD_DE_MODULOS = 5;
    const char* nombresDeModulos[CANTIDAD_DE_MODULOS] = {
        "GestorArchivos", "AdminAmistades", "Autenticacion", "VisorPerfiles", "GestorMensajes"
    };
    unsigned long long iteracionesPorModulo[CANTIDAD_DE_MODULOS] = {
        baseDatos.getIteraciones(), moduloAmistades.getIteraciones(), moduloAcceso.getIteraciones(),
        moduloPerfiles.getIteraciones(), moduloMensajes.getIteraciones()
    };
    size_t memoriaPorModulo[CANTIDAD_DE_MODULOS] = {
        baseDatos.getMemoriaReservada(), moduloAmistades.getMemoriaReservada(),
        moduloAcceso.getMemoriaReservada(), moduloPerfiles.getMemoriaReservada(),
        moduloMensajes.getMemoriaReservada()
    };

    monitor.mostrarReporte(nombresDeModulos, iteracionesPorModulo, memoriaPorModulo, CANTIDAD_DE_MODULOS);

    baseDatos.reiniciarIteraciones();
    moduloAmistades.reiniciarIteraciones();
    moduloAcceso.reiniciarIteraciones();
    moduloPerfiles.reiniciarIteraciones();
    moduloMensajes.reiniciarIteraciones();
}

void Menu::ejecutarOpcion(int opcionElegida) {
    int idPropioComoNumero = convertirTextoANumero(moduloAcceso.getIdDelUsuarioActivo());

    if (opcionElegida == 1) {
        moduloAcceso.mostrarMejoresAmigos(moduloAmistades);

    } else if (opcionElegida == 2) {
        char nombreBuscado[100];
        cout << "Nombre de la persona a visitar: ";
        cin.getline(nombreBuscado, 100);

        char idEncontrado[100];
        bool encontrado = moduloPerfiles.buscarPerfilPorNombre(baseDatos, nombreBuscado, idEncontrado);

        if (!encontrado) {
            cout << "No existe ningun usuario con ese nombre." << endl;
        } else {
            moduloPerfiles.mostrarCarrusel(baseDatos, moduloAmistades,
                moduloAcceso.getIdDelUsuarioActivo(), idEncontrado);

            cout << "Dar like a una foto (nombre exacto, o vacio para omitir): ";
            char nombreDeFoto[100];
            cin.getline(nombreDeFoto, 100);
            if (nombreDeFoto[0] != '\0') {
                moduloPerfiles.darLike(baseDatos, idEncontrado, nombreDeFoto);
            }
        }

    } else if (opcionElegida == 3) {
        moduloAmistades.mostrarMenu(idPropioComoNumero, cin, cout);

    } else if (opcionElegida == 4) {
        moduloAmistades.mostrarMenuSugerencias(idPropioComoNumero, cin, cout);

    } else if (opcionElegida == 5) {
        moduloMensajes.mostrarMenu(moduloAmistades, idPropioComoNumero, Fecha(18, 10, 2026), cin, cout);

    } else {
        cout << "Opcion invalida." << endl;
    }
}

bool Menu::iniciar(const char* rutaIndiceUsuarios) {
    bool indiceCargado = baseDatos.cargarIndiceUsuarios(rutaIndiceUsuarios);
    if (!indiceCargado) {
        cout << "No se pudo cargar el indice de usuarios. Cerrando." << endl;
        return false;
    }

    cargarUsuariosEnRedDeAmistades();

    bool accesoAutorizado = moduloAcceso.iniciarSesion(baseDatos);
    if (!accesoAutorizado) {
        cout << "Acceso no autorizado." << endl;
        return false;
    }

    bool ejecutando = true;
    while (ejecutando) {

        cout << "\n===== MENU PRINCIPAL =====" << endl;
        cout << "1. Ver mi amigo de mayor prioridad" << endl;
        cout << "2. Visitar el perfil de alguien" << endl;
        cout << "3. Administrar amistades" << endl;
        cout << "4. Ver sugerencias de amistad" << endl;
        cout << "5. Mensajes" << endl;
        cout << "0. Salir" << endl;
        cout << "Opcion: ";

        int opcionElegida = -1;
        if (!(cin >> opcionElegida)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ingresa un numero." << endl;
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (opcionElegida == 0) {
            ejecutando = false;
        } else {
            ejecutarOpcion(opcionElegida);
            mostrarReporteDeTodos();
            monitor.limpiarPantalla();
        }
    }

    return true;
}
