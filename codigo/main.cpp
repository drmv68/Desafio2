#include <iostream>

// Modulos principales (usando comillas dobles, rutas relativas)
#include "core/monitor_recursos.h"
#include "core/gestor_archivos.h"
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
//
// FASE MACRO: el flujo esta trazado pero las llamadas reales
// siguen comentadas hasta que cada modulo tenga su logica.
// ============================================================

int main() {
    // Bloque try/catch generalizado para atrapar errores de lectura o ejecucion
    try {
        // 1. Instanciamos las clases macro
        GestorArchivos baseDatos;
        Autenticacion moduloAcceso;
        AdminAmistades moduloAmistades;
        VisorPerfiles moduloPerfiles;
        GestorMensajes moduloMensajes;

        // 2. Llamamos a la validacion de inicio (retorna booleano)
        bool accesoAutorizado = false;
        // accesoAutorizado = moduloAcceso.iniciarSesion(baseDatos);

        if (accesoAutorizado) {
            // 3. Bucle del Menu Principal
            bool ejecutando = true;
            while (ejecutando) {
                // Aqui iran las llamadas a los otros modulos dinamicos segun la opcion
                // Al final de cada opcion: MonitorRecursos::imprimirReporte();
                ejecutando = false; // placeholder para que el bucle no quede infinito
            }
        } else {
            cout << "Acceso no autorizado (logica de autenticacion pendiente)." << endl;
        }

    } catch (...) {
        cout << "Error critico atrapado. Sincronizando y cerrando de forma segura." << endl;
    }

    return 0;
}
