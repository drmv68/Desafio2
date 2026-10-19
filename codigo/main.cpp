#include <iostream>
#include "core/gestor_archivos.h"
#include "core/monitor_recursos.h"
#include "modulos_dinamicos/autenticacion.h"
#include "modulos_dinamicos/admin_amistades.h"
#include "modulos_dinamicos/visor_perfiles.h"
#include "modulos_dinamicos/gestor_mensajes.h"
#include "modulos_dinamicos/menu.h"

using namespace std;

int main() {
    try {
        GestorArchivos baseDatos;
        Autenticacion moduloAcceso;
        AdminAmistades moduloAmistades;
        VisorPerfiles moduloPerfiles;
        GestorMensajes moduloMensajes;
        MonitorRecursos monitor;

        Menu menuPrincipal(baseDatos, moduloAcceso, moduloAmistades, moduloPerfiles, moduloMensajes, monitor);
        menuPrincipal.iniciar("estaticos_y_db/indice.txt");

    } catch (...) {
        cout << "Error critico atrapado, sincronizando y cerrando." << endl;
    }

    return 0;
}
