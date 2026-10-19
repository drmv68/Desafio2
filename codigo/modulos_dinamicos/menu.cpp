#include "../core/monitor_recursos.h"
#include "menu.h"
#include <iostream>
#include <limits>
#include <stdexcept>

namespace {
int numero(const char *texto) {
    int n = 0;
    while (*texto) {
        MonitorRecursos::registrarIteracion(0);
        if (*texto < '0' || *texto > '9' ||
            n > (std::numeric_limits<int>::max() - (*texto - '0')) / 10)
            throw std::invalid_argument("ID de usuario invalido");
        n = n * 10 + (*texto++ - '0');
    }
    return n;
}
} // namespace
Menu::Menu(GestorArchivos &b, Autenticacion &a, AdminAmistades &r, VisorPerfiles &p,
           GestorMensajes &m, MonitorRecursos &monitorInicial)
    : baseDatos(b), moduloAcceso(a), moduloAmistades(r), moduloPerfiles(p), moduloMensajes(m),
      monitor(monitorInicial) {
}
void Menu::cargarUsuariosEnRedDeAmistades() {
    for (int i = 0; i < baseDatos.getTotalUsuarios(); ++i) {
        MonitorRecursos::registrarIteracion(0);
        const UsuarioLigero &u = baseDatos.getUsuarioLigero(i);
        if (!moduloAmistades.cargarUsuario(numero(u.getIdUsuario()), u.getNombreUsuario()))
            throw std::runtime_error("Usuario duplicado o invalido al cargar la red");
    }
}
void Menu::mostrarReporteDeTodos() {
    MonitorRecursos::pausarConteo(true);
    const char *nombres[] = {"GestorArchivos", "AdminAmistades", "Autenticacion", "VisorPerfiles",
                             "GestorMensajes"};
    unsigned long long iteraciones[] = {
        baseDatos.getIteraciones(), moduloAmistades.getIteraciones(), moduloAcceso.getIteraciones(),
        moduloPerfiles.getIteraciones(), moduloMensajes.getIteraciones()};
    std::size_t memoria[] = {baseDatos.getMemoriaReservada(), moduloAmistades.getMemoriaReservada(),
                             moduloAcceso.getMemoriaReservada(),
                             moduloPerfiles.getMemoriaReservada(),
                             moduloMensajes.getMemoriaReservada()};
    MemoriaTemporal locales(sizeof(nombres) + sizeof(iteraciones) + sizeof(memoria) + sizeof(this));
    monitor.mostrarReporte(nombres, iteraciones, memoria, 5);
    baseDatos.reiniciarIteraciones();
    moduloAmistades.reiniciarIteraciones();
    moduloAcceso.reiniciarIteraciones();
    moduloPerfiles.reiniciarIteraciones();
    moduloMensajes.reiniciarIteraciones();
    MonitorRecursos::pausarConteo(false);
}
void Menu::terminarOperacion(void *contexto) {
    MemoriaTemporal locales(2 * sizeof(void *));
    Menu &menu = *static_cast<Menu *>(contexto);
    if (!menu.baseDatos.guardarEstado(menu.moduloAmistades, menu.moduloMensajes))
        throw std::runtime_error("No se pudo guardar: se conservaron los archivos anteriores. "
                                 "Cambios pendientes en memoria");
    menu.mostrarReporteDeTodos();
}
void Menu::ejecutarOpcion(int opcion) {
    int activo = numero(moduloAcceso.getIdDelUsuarioActivo());
    MemoriaTemporal locales(sizeof(opcion) + sizeof(activo) + sizeof(this));
    if (opcion == 1) {
        moduloAcceso.mostrarMejoresAmigos(baseDatos, moduloAmistades, true, std::cout);
        terminarOperacion(this);
    } else if (opcion == 2) {
        char nombre[1025], id[100];
        MemoriaTemporal textos(sizeof(nombre) + sizeof(id));
        std::cout << "Nombre de la persona a visitar: ";
        if (!std::cin.getline(nombre, sizeof nombre)) {
            if (!std::cin.eof()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            std::cout << "Nombre invalido.\n";
        } else if (!moduloPerfiles.buscarPerfilPorNombre(baseDatos, nombre, id, sizeof id))
            std::cout << "No existe ese usuario.\n";
        else
            moduloPerfiles.mostrarCarrusel(baseDatos, moduloAmistades,
                                           moduloAcceso.getIdDelUsuarioActivo(), id, std::cin,
                                           std::cout, terminarOperacion, this);
        terminarOperacion(this);
    } else if (opcion == 3)
        moduloAmistades.mostrarMenu(activo, std::cin, std::cout, terminarOperacion, this);
    else if (opcion == 4)
        moduloAmistades.mostrarMenuSugerencias(activo, std::cin, std::cout, terminarOperacion,
                                               this);
    else if (opcion == 5) {
        Fecha actual = Fecha::actual();
        MemoriaTemporal fecha(sizeof(actual));
        moduloMensajes.mostrarMenu(moduloAmistades, activo, actual, std::cin, std::cout,
                                   terminarOperacion, this);
    } else
        std::cout << "Opcion invalida.\n";
}
bool Menu::iniciar(const char *rutaIndice) {
    char ruta[1200];
    MemoriaTemporal locales(sizeof(ruta) + sizeof(rutaIndice) + sizeof(this) + sizeof(int));
    if (!baseDatos.cargarIndiceUsuarios(rutaIndice)) {
        std::cout << "Indice inexistente o invalido: " << rutaIndice << '\n';
        return false;
    }
    cargarUsuariosEnRedDeAmistades();
    if (!baseDatos.construirRuta("amistades.txt", ruta, sizeof ruta) ||
        !baseDatos.cargarAmistades(ruta, moduloAmistades) ||
        !baseDatos.construirRuta("solicitudes.txt", ruta, sizeof ruta) ||
        !baseDatos.cargarSolicitudes(ruta, moduloAmistades) ||
        !baseDatos.construirRuta("mensajes.txt", ruta, sizeof ruta) ||
        !baseDatos.cargarMensajes(ruta, moduloAmistades, moduloMensajes)) {
        std::cout
            << "Archivos de relaciones o mensajes incompletos o invalidos. No se modificaron.\n";
        return false;
    }
    if (!moduloAcceso.iniciarSesion(baseDatos))
        return false;
    moduloAcceso.mostrarMejoresAmigos(baseDatos, moduloAmistades, false, std::cout);
    mostrarReporteDeTodos();
    MonitorRecursos::registrarComponente(2);
    while (std::cin) {
        MonitorRecursos::registrarIteracion(0);
        std::cout
            << "\nMENU PRINCIPAL\n1. Actualizar fotos de amigos (una vez)\n2. Visitar perfil\n"
               "3. Administrar amistades\n4. Sugerencias de amistad\n5. Mensajes\n0. "
               "Salir\nOpcion: ";
        int opcion = -1;
        if (!(std::cin >> opcion)) {
            if (std::cin.eof())
                break;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ingresa un numero.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (!opcion)
            break;
        ejecutarOpcion(opcion);
    }
    if (!baseDatos.guardarEstado(moduloAmistades, moduloMensajes))
        throw std::runtime_error("No fue posible guardar al cerrar");
    return true;
}
