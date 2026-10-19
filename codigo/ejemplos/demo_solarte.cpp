#include "../modulos_dinamicos/gestor_mensajes.h"
#include <iostream>
#include <limits>
#include <exception>

int main() {
    try {
        AdminAmistades red;
        GestorMensajes mensajes;
        red.cargarUsuario(10, "juan"); red.cargarUsuario(20, "daniel");
        red.cargarUsuario(30, "ana"); red.cargarUsuario(40, "luis");
        red.cargarUsuario(50, "maria");
        red.cargarAmistad(10, 20); red.cargarAmistad(20, 30);
        red.cargarSolicitud(40, 10);
        mensajes.cargarMensaje(red, 1, 20, 10, Fecha(1, 8, 2026), "Hola Juan", false);
        mensajes.cargarMensaje(red, 2, 20, 10, Fecha(30, 9, 2026), "Segundo mensaje", false);
        int opcion = -1;
        std::cout << "DEMO EN MEMORIA | usuario: juan | fecha de ejemplo: 16/10/2026\n";
        while (opcion != 0 && std::cin) {
            std::cout << "\n1. Amistades\n2. Sugerencias\n3. Mensajes\n0. Salir\nOpcion: ";
            int leida = -1;
            if (!(std::cin >> leida)) {
                if (std::cin.eof()) break;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            opcion = leida;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (opcion == 1) red.mostrarMenu(10, std::cin, std::cout);
            else if (opcion == 2) red.mostrarMenuSugerencias(10, std::cin, std::cout);
            else if (opcion == 3) mensajes.mostrarMenu(red, 10, Fecha(16, 10, 2026), std::cin, std::cout);
            else if (opcion != 0) std::cout << "Opcion invalida.\n";
        }
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
