#include <iostream>
#include <exception>
#include <stdexcept>
#include "core/gestor_archivos.h"
#include "core/monitor_recursos.h"
#include "modulos_dinamicos/menu.h"

int main(int argc, char **argv) {
    try {
        GestorArchivos base;
        Autenticacion acceso;
        AdminAmistades amistades;
        VisorPerfiles perfiles;
        GestorMensajes mensajes;
        MonitorRecursos monitor;
        Menu menu(base, acceso, amistades, perfiles, mensajes, monitor);
        char ruta[1200];
        MemoriaTemporal locales(sizeof(menu) + sizeof(ruta) + sizeof(argc) + sizeof(argv) +
                                2 * sizeof(int));
        int ultimo = -1;
        for (int i = 0; argv[0][i]; ++i) {
            MonitorRecursos::registrarIteracion(0);
            if (argv[0][i] == '/' || argv[0][i] == '\\')
                ultimo = i;
        }
        if (ultimo + 1 >= 1100)
            throw std::runtime_error("Ruta del ejecutable demasiado larga");
        int pos = 0;
        for (; pos <= ultimo; ++pos) {
            MonitorRecursos::registrarIteracion(0);
            ruta[pos] = argv[0][pos];
        }
        for (const char *p = "estaticos_y_db/indice.txt"; *p; ++p) {
            MonitorRecursos::registrarIteracion(0);
            ruta[pos++] = *p;
        }
        ruta[pos] = '\0';
        return menu.iniciar(argc > 1 ? argv[1] : ruta) ? 0 : 1;
    } catch (const std::exception &error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Error inesperado.\n";
        return 1;
    }
}
