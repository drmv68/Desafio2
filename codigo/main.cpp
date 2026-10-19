#include <iostream>
#include <limits>

// Modulos principales (usando comillas dobles, rutas relativas)
#include "core/monitor_recursos.h"
#include "core/gestor_archivos.h"
#include "core/estructuras_base.h"
#include "modulos_dinamicos/autenticacion.h"
#include "modulos_dinamicos/admin_amistades.h"
#include "modulos_dinamicos/visor_perfiles.h"
#include "modulos_dinamicos/gestor_mensajes.h"

using namespace std;

// ============================================================
// main.cpp
// Modulo 0 del analisis: limpio, solo llama, sincroniza y
// atrapa errores. Toda la logica pesada vive en los modulos
// dinamicos y en core/.
// ============================================================

// Convierte el id de texto (char*) que usa GestorArchivos en el numero
// entero que usan AdminAmistades y GestorMensajes para identificar
// usuarios. Es el mismo puente manual que ya usan Autenticacion y
// VisorPerfiles (cada modulo tiene su propia copia a proposito).
static int convertirTextoANumero(const char* texto) {
    int numero = 0;
    int posicion = 0;
    while (texto[posicion] != '\0') {
        numero = numero * 10 + (texto[posicion] - '0');
        posicion++;
    }
    return numero;
}

// Le pregunta a cada modulo cuanto gasto, le pasa esos numeros a
// MonitorRecursos para que imprima el reporte, y despues reinicia el
// contador de cada modulo para la siguiente operacion del menu.
static void mostrarReporteDeTodos(MonitorRecursos& monitor, GestorArchivos& baseDatos,
                                   AdminAmistades& red, Autenticacion& acceso,
                                   VisorPerfiles& perfiles, GestorMensajes& mensajes) {
    const int CANTIDAD_DE_MODULOS = 5;
    const char* nombresDeModulos[CANTIDAD_DE_MODULOS] = {
        "GestorArchivos", "AdminAmistades", "Autenticacion", "VisorPerfiles", "GestorMensajes"
    };
    unsigned long long iteracionesPorModulo[CANTIDAD_DE_MODULOS] = {
        baseDatos.getIteraciones(), red.getIteraciones(), acceso.getIteraciones(),
        perfiles.getIteraciones(), mensajes.getIteraciones()
    };
    size_t memoriaPorModulo[CANTIDAD_DE_MODULOS] = {
        baseDatos.getMemoriaReservada(), red.getMemoriaReservada(), acceso.getMemoriaReservada(),
        perfiles.getMemoriaReservada(), mensajes.getMemoriaReservada()
    };

    monitor.mostrarReporte(nombresDeModulos, iteracionesPorModulo, memoriaPorModulo, CANTIDAD_DE_MODULOS);

    baseDatos.reiniciarIteraciones();
    red.reiniciarIteraciones();
    acceso.reiniciarIteraciones();
    perfiles.reiniciarIteraciones();
    mensajes.reiniciarIteraciones();
}

int main() {
    // Bloque try/catch generalizado para atrapar errores de lectura o ejecucion
    try {
        // 1. Instanciamos las clases macro
        GestorArchivos baseDatos;
        Autenticacion moduloAcceso;
        AdminAmistades moduloAmistades;
        VisorPerfiles moduloPerfiles;
        GestorMensajes moduloMensajes;
        MonitorRecursos monitor;

        // 2. Cargamos el indice ligero de usuarios
        bool indiceCargado = baseDatos.cargarIndiceUsuarios("estaticos_y_db/indice.txt");
        if (!indiceCargado) {
            cout << "No se pudo cargar el indice de usuarios. Cerrando." << endl;
            return 0;
        }

        // 3. Puente entre GestorArchivos (ids en texto) y AdminAmistades (ids
        // en numero): cada usuario del indice se "da de alta" tambien en la
        // red de amistades.
        // PENDIENTE: todavia no existe un archivo que diga "quien es amigo de
        // quien" (algo como estaticos_y_db/amistades.txt), asi que por ahora
        // todos entran sin amigos hasta que definamos ese formato y carguemos
        // tambien las cargarAmistad() aqui.
        int posicionDeCarga = 0;
        while (posicionDeCarga < baseDatos.getTotalUsuarios()) {
            const UsuarioLigero& usuarioActual = baseDatos.getUsuarioLigero(posicionDeCarga);
            int idComoNumero = convertirTextoANumero(usuarioActual.getIdUsuario());
            moduloAmistades.cargarUsuario(idComoNumero, usuarioActual.getNombreUsuario());
            posicionDeCarga++;
        }

        // 4. Llamamos a la validacion de inicio (retorna booleano)
        bool accesoAutorizado = moduloAcceso.iniciarSesion(baseDatos);

        if (accesoAutorizado) {
            // 5. Bucle del Menu Principal
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
                    moduloMensajes.mostrarMenu(moduloAmistades, idPropioComoNumero,
                                                Fecha(18, 10, 2026), cin, cout);

                } else if (opcionElegida == 0) {
                    ejecutando = false;

                } else {
                    cout << "Opcion invalida." << endl;
                }

                if (opcionElegida != 0) {
                    mostrarReporteDeTodos(monitor, baseDatos, moduloAmistades, moduloAcceso,
                                          moduloPerfiles, moduloMensajes);
                    monitor.limpiarPantalla();
                }
            }
        } else {
            cout << "Acceso no autorizado." << endl;
        }

    } catch (...) {
        cout << "Error critico atrapado. Sincronizando y cerrando de forma segura." << endl;
    }

    return 0;
}
