#include <iostream>

#include "core/monitor_recursos.h"
#include "core/gestor_archivos.h"
#include "modulos_dinamicos/autenticacion.h"
#include "modulos_dinamicos/admin_amistades.h"
#include "modulos_dinamicos/visor_perfiles.h"
#include "modulos_dinamicos/gestor_mensajes.h"

using namespace std;

int main() {

    try {
        // 1. Instanciamos las clases
        GestorArchivos baseDatos;
        Autenticacion moduloAcceso;
        AdminAmistades moduloAmistades;
        VisorPerfiles moduloPerfiles;
        GestorMensajes moduloMensajes;
        bool accesoAutorizado = false;

        if (accesoAutorizado) {

            bool ejecutando = true;
            while (ejecutando) {
                // iran llamadas a los otros modulos dinamicos segun la opcion
                // Al final de cada opcion: MonitorRecursos::imprimirReporte();
                ejecutando = false;
            }
        } else {
            cout << "Acceso no autorizado (logica de autenticacion pendiente)." << endl;
        }

    } catch (...) {
        cout << "Error critico atrapado" << endl;
    }

    return 0;
}
