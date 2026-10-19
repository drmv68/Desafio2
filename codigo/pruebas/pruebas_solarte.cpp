#include "../modulos_dinamicos/gestor_mensajes.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

void comprobar(bool condicion, int linea) {
    if (!condicion) {
        std::cerr << "Fallo en linea " << linea << '\n';
        throw std::runtime_error("Una comprobacion no se cumplio");
    }
}
#define exigir(condicion) comprobar((condicion), __LINE__)
void cargarCinco(AdminAmistades& red) {
    exigir(red.cargarUsuario(10, "juan"));
    exigir(red.cargarUsuario(20, "daniel"));
    exigir(red.cargarUsuario(30, "ana"));
    exigir(red.cargarUsuario(40, "luis"));
    exigir(red.cargarUsuario(50, "maria"));
    exigir(red.cargarAmistad(10, 20));
    exigir(red.cargarAmistad(20, 30));
}
void solicitudes() {
    AdminAmistades red(2);
    cargarCinco(red);
    exigir(!red.cargarUsuario(10, "otro") && !red.cargarUsuario(99, "juan"));
    exigir(!red.cargarUsuario(0, "nadie") && !red.cargarUsuario(99, "   "));
    exigir(red.sonAmigos(10, 20) && red.sonAmigos(20, 10));
    exigir(!red.envioDeSolicitud(10, 10) && !red.envioDeSolicitud(10, 20));
    exigir(!red.envioDeSolicitud(10, "desconocido"));
    exigir(red.envioDeSolicitud(10, "ana"));
    exigir(!red.envioDeSolicitud(10, 30) && !red.envioDeSolicitud(30, 10));
    exigir(!red.aceptarSolicitud(10, 30));
    exigir(red.aceptarSolicitud(30, 10));
    exigir(red.sonAmigos(10, 30) && red.sonAmigos(30, 10));
    exigir(!red.haySolicitud(10, 30));
    exigir(red.envioDeSolicitud(40, 10) && red.rechazarSolicitud(10, 40));
    exigir(!red.haySolicitud(40, 10) && red.envioDeSolicitud(10, 40));
    exigir(!red.aceptarSolicitud(10, 999));
}
void sugerencias() {
    AdminAmistades red;
    for (int i = 1; i <= 50; ++i) {
        char nombre[] = "usuario00";
        nombre[7] = static_cast<char>('0' + i / 10);
        nombre[8] = static_cast<char>('0' + i % 10);
        exigir(red.cargarUsuario(i * 10, nombre));
    }
    for (int i = 2; i <= 4; ++i) exigir(red.cargarAmistad(10, i * 10));
    for (int i = 5; i <= 20; ++i) {
        exigir(red.cargarAmistad(i * 10, 20));
        if (i <= 14) exigir(red.cargarAmistad(i * 10, 30));
        if (i <= 8) exigir(red.cargarAmistad(i * 10, 40));
    }
    exigir(red.envioDeSolicitud(10, 50) && red.envioDeSolicitud(60, 10));
    int ids[20], comunes[20];
    exigir(red.sugerirAmigos(10, 12, ids, comunes) == 12);
    for (int i = 0; i < 12; ++i) exigir(ids[i] == (i + 7) * 10);
    exigir(comunes[0] == 3 && comunes[2] == 2 && comunes[8] == 1);
    exigir(red.sugerirAmigos(10, 20, ids, comunes) == 14);
    exigir(red.sugerirAmigos(500, 12, ids, comunes) == 0);
    exigir(red.sugerirAmigos(10, 0, ids, comunes) == -1);
    exigir(red.sugerirAmigos(10, 21, ids, comunes) == -1);
    exigir(red.envioDeSolicitud(10, ids[0]));
}
void validacionMensajes() {
    AdminAmistades red;
    cargarCinco(red);
    GestorMensajes mensajes;
    Fecha fecha(29, 2, 2024);
    bool invalida = false;
    try { Fecha imposible(29, 2, 2026); } catch (const std::invalid_argument&) { invalida = true; }
    exigir(invalida && Fecha(28, 2, 2026) <= Fecha(1, 3, 2026));
    exigir(!mensajes.enviarMensaje(red, 10, "ana", "Hola", fecha));
    exigir(!mensajes.enviarMensaje(red, 10, "daniel", "", fecha));
    exigir(!mensajes.enviarMensaje(red, 10, "daniel", " \t\n", fecha));
    exigir(!mensajes.enviarMensaje(red, 10, "daniel", nullptr, fecha));
    char texto[1027];
    for (int i = 0; i < 513; ++i) { texto[2*i] = '\xC3'; texto[2*i+1] = '\xA1'; }
    texto[1024] = '\0';
    exigir(mensajes.enviarMensaje(red, 10, "daniel", texto, fecha));
    texto[1024] = '\xC3'; texto[1026] = '\0';
    exigir(!mensajes.enviarMensaje(red, 10, "daniel", texto, fecha));
    exigir(!mensajes.enviarMensaje(red, 10, "daniel", "\xC3", fecha));
    exigir(mensajes.getCantidad() == 1);
    const Mensaje* original = mensajes.getMensaje(0);
    Mensaje copia(*original);
    exigir(copia.getTexto() != original->getTexto() && copia.getId() == original->getId());
}
void fechasYLectura() {
    AdminAmistades red;
    cargarCinco(red);
    GestorMensajes mensajes;
    exigir(mensajes.cargarMensaje(red, 5, 10, 20, Fecha(1, 10, 2026), "fuera despues", false));
    exigir(mensajes.cargarMensaje(red, 3, 10, 20, Fecha(30, 9, 2026), "fin", false));
    exigir(mensajes.cargarMensaje(red, 2, 10, 20, Fecha(1, 8, 2026), "inicio", false));
    exigir(mensajes.cargarMensaje(red, 1, 10, 20, Fecha(31, 7, 2026), "fuera antes", true));
    exigir(mensajes.cargarMensaje(red, 4, 20, 10, Fecha(1, 9, 2026), "otro receptor", false));
    exigir(!mensajes.cargarMensaje(red, 2, 10, 20, Fecha(), "duplicado", false));
    std::ostringstream salida;
    exigir(mensajes.buscarPorRangoDeFechas(red, 20, Fecha(1, 8, 2026), Fecha(30, 9, 2026), salida) == 2);
    exigir(salida.str().find("inicio") < salida.str().find("fin"));
    exigir(!mensajes.getMensaje(1)->estaLeido());
    exigir(mensajes.buscarPorRangoDeFechas(red, 20, Fecha(30, 9, 2026), Fecha(30, 9, 2026), salida) == 1);
    exigir(mensajes.buscarPorRangoDeFechas(red, 20, Fecha(1, 10, 2026), Fecha(1, 8, 2026), salida) == -1);
    std::ostringstream fallida;
    fallida.setstate(std::ios::badbit);
    exigir(mensajes.leerNuevos(red, 20, fallida) == -1);
    exigir(!mensajes.getMensaje(1)->estaLeido());
    exigir(mensajes.leerNuevos(red, 20, salida) == 3);
    exigir(mensajes.leerNuevos(red, 20, salida) == 0);
    exigir(!mensajes.getMensaje(2)->estaLeido());
    exigir(mensajes.enviarMensaje(red, 10, "daniel", "siguiente id", Fecha(2, 10, 2026)));
    exigir(mensajes.getMensaje(5)->getId() == 6);
}
void menus() {
    AdminAmistades red;
    cargarCinco(red);
    GestorMensajes mensajes;
    std::ostringstream salida;
    std::istringstream amistades("texto\n1\nluis\n0\n");
    red.mostrarMenu(10, amistades, salida);
    exigir(red.haySolicitud(10, 40));
    std::istringstream sugeridas("12\n1\n");
    red.mostrarMenuSugerencias(10, sugeridas, salida);
    exigir(red.haySolicitud(10, 30));
    std::istringstream chat("1\ndaniel\nHola Daniel\n0\n");
    mensajes.mostrarMenu(red, 10, Fecha(16, 10, 2026), chat, salida);
    exigir(mensajes.getCantidad() == 1);
    std::istringstream leer("2\n0\n");
    mensajes.mostrarMenu(red, 20, Fecha(16, 10, 2026), leer, salida);
    exigir(mensajes.getMensaje(0)->estaLeido());
    std::istringstream eof("");
    red.mostrarMenu(10, eof, salida);
    exigir(red.getMemoriaReservada() > sizeof(red));
    exigir(mensajes.getMemoriaReservada() > sizeof(mensajes));
    exigir(red.getIteraciones() > 0 && mensajes.getIteraciones() > 0);
    red.reiniciarIteraciones(); mensajes.reiniciarIteraciones();
    exigir(red.getIteraciones() == 0 && mensajes.getIteraciones() == 0);
}
int main(int argc, char** argv) {
    if (argc != 2) return 2;
    try {
        switch (argv[1][0]) {
        case '1': solicitudes(); break;
        case '2': sugerencias(); break;
        case '3': validacionMensajes(); break;
        case '4': fechasYLectura(); break;
        case '5': menus(); break;
        default: return 2;
        }
        std::cout << "Prueba " << argv[1] << " correcta.\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n'; return 1;
    }
}
