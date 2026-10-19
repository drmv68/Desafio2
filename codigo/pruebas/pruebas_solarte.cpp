#include "../modulos_dinamicos/gestor_mensajes.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <chrono>
#include "../modulos_dinamicos/menu.h"

void comprobar(bool condicion, int linea) {
    if (!condicion) {
        std::cerr << "Fallo en linea " << linea << '\n';
        throw std::runtime_error("Una comprobacion no se cumplio");
    }
}
#define exigir(condicion) comprobar((condicion), __LINE__)

// Los archivos se prueban en una copia temporal exclusiva; nunca sobre la base real.
class DatosPrueba {
    std::filesystem::path carpeta;

  public:
    DatosPrueba() {
        carpeta = std::filesystem::temp_directory_path() /
                  ("udeabook-prueba-" +
                   std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        exigir(std::filesystem::create_directory(carpeta));
        try {
            for (const auto &archivo : std::filesystem::directory_iterator(DATOS_PRUEBA_ORIGEN))
                std::filesystem::copy(archivo.path(), carpeta / archivo.path().filename(),
                                      std::filesystem::copy_options::recursive |
                                          std::filesystem::copy_options::overwrite_existing);
        } catch (...) {
            std::error_code error;
            std::filesystem::remove_all(carpeta, error);
            throw;
        }
    }
    ~DatosPrueba() {
        std::error_code error;
        std::filesystem::remove_all(carpeta, error);
    }
    std::string ruta(const char *archivo) const { return (carpeta / archivo).string(); }
    void cargar(GestorArchivos &base, AdminAmistades &red, GestorMensajes &mensajes) const {
        exigir(base.cargarIndiceUsuarios(ruta("indice.txt").c_str()));
        for (int i = 0; i < base.getTotalUsuarios(); ++i) {
            const UsuarioLigero &u = base.getUsuarioLigero(i);
            exigir(red.cargarUsuario(std::stoi(u.getIdUsuario()), u.getNombreUsuario()));
        }
        exigir(base.cargarAmistades(ruta("amistades.txt").c_str(), red));
        exigir(base.cargarSolicitudes(ruta("solicitudes.txt").c_str(), red));
        exigir(base.cargarMensajes(ruta("mensajes.txt").c_str(), red, mensajes));
    }
};
class ConsolaPrueba {
    std::istringstream entrada;
    std::ostringstream salida;
    std::streambuf *anteriorEntrada;
    std::streambuf *anteriorSalida;

  public:
    explicit ConsolaPrueba(const char *texto)
        : entrada(texto), anteriorEntrada(std::cin.rdbuf(entrada.rdbuf())),
          anteriorSalida(std::cout.rdbuf(salida.rdbuf())) {
        std::cin.clear();
    }
    ~ConsolaPrueba() {
        std::cin.rdbuf(anteriorEntrada);
        std::cout.rdbuf(anteriorSalida);
        std::cin.clear();
    }
    std::string texto() const { return salida.str(); }
};
std::string leerArchivo(const std::string &ruta) {
    std::ifstream archivo(ruta);
    std::ostringstream salida;
    salida << archivo.rdbuf();
    return salida.str();
}
void cargarCinco(AdminAmistades &red) {
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

    DatosPrueba datos;
    GestorArchivos base;
    AdminAmistades redReal;
    GestorMensajes historial;
    datos.cargar(base, redReal, historial);
    UsuarioLigero copia = base.getUsuarioLigero(0);
    copia = copia;
    copia.setNombreUsuario(copia.getNombreUsuario()); // Antes liberaba el origen antes de copiarlo.
    exigir(std::string(copia.getNombreUsuario()) == "daniel mosquera");
    std::ofstream malo(datos.ruta("incorrecto.txt"));
    malo << "-1\n";
    malo.close();
    exigir(!base.cargarIndiceUsuarios(datos.ruta("incorrecto.txt").c_str()));
    exigir(base.getTotalUsuarios() == 8); // Una carga fallida no destruye el indice anterior.
    std::ofstream cortado(datos.ruta("cortado.txt"));
    cortado << "2\n1001;1002\n";
    cortado.close();
    AdminAmistades vacia;
    exigir(vacia.cargarUsuario(1001, "uno") && vacia.cargarUsuario(1002, "dos"));
    exigir(!base.cargarAmistades(datos.ruta("cortado.txt").c_str(), vacia));
}
void sugerencias() {
    AdminAmistades red;
    for (int i = 1; i <= 50; ++i) {
        char nombre[] = "usuario00";
        nombre[7] = static_cast<char>('0' + i / 10);
        nombre[8] = static_cast<char>('0' + i % 10);
        exigir(red.cargarUsuario(i * 10, nombre));
    }
    for (int i = 2; i <= 4; ++i)
        exigir(red.cargarAmistad(10, i * 10));
    for (int i = 5; i <= 20; ++i) {
        exigir(red.cargarAmistad(i * 10, 20));
        if (i <= 14)
            exigir(red.cargarAmistad(i * 10, 30));
        if (i <= 8)
            exigir(red.cargarAmistad(i * 10, 40));
    }
    exigir(red.envioDeSolicitud(10, 50) && red.envioDeSolicitud(60, 10));
    int ids[20], comunes[20];
    exigir(red.sugerirAmigos(10, 12, ids, comunes) == 12);
    for (int i = 0; i < 12; ++i)
        exigir(ids[i] == (i + 7) * 10);
    exigir(comunes[0] == 3 && comunes[2] == 2 && comunes[8] == 1);
    exigir(red.sugerirAmigos(10, 20, ids, comunes) == 14);
    exigir(red.sugerirAmigos(500, 12, ids, comunes) == 0);
    exigir(red.sugerirAmigos(10, 0, ids, comunes) == -1);
    exigir(red.sugerirAmigos(10, 21, ids, comunes) == -1);
    exigir(red.envioDeSolicitud(10, ids[0]));

    DatosPrueba datos;
    GestorArchivos base;
    AdminAmistades redReal;
    GestorMensajes historial;
    datos.cargar(base, redReal, historial);
    Autenticacion acceso;
    std::ostringstream fotos;
    std::istringstream credenciales("1001\npass123\n");
    exigir(acceso.iniciarSesion(base, credenciales, fotos));
    exigir(acceso.mostrarMejoresAmigos(base, redReal, false, fotos));
    int anteriores[3];
    for (int i = 0; i < 3; ++i) {
        anteriores[i] = acceso.getAmigoMostrado(i);
        exigir(redReal.sonAmigos(1001, anteriores[i]));
        for (int j = 0; j < i; ++j)
            exigir(anteriores[i] != anteriores[j]);
    }
    exigir(fotos.str().find("/profile.png") != std::string::npos);
    exigir(acceso.mostrarMejoresAmigos(base, redReal, true, fotos));
    bool distinto = false;
    for (int i = 0; i < 3; ++i) {
        bool estaba = false;
        for (int j = 0; j < 3; ++j)
            if (acceso.getAmigoMostrado(i) == anteriores[j])
                estaba = true;
        if (!estaba)
            distinto = true;
    }
    exigir(distinto && !acceso.mostrarMejoresAmigos(base, redReal, true, fotos));
    std::istringstream incorrectas("1001\nincorrecta\n");
    exigir(!acceso.iniciarSesion(base, incorrectas, fotos));
    exigir(acceso.getIdDelUsuarioActivo() == nullptr);
}
void validacionMensajes() {
    AdminAmistades red;
    cargarCinco(red);
    GestorMensajes mensajes;
    Fecha fecha(29, 2, 2024);
    bool invalida = false;
    try {
        Fecha imposible(29, 2, 2026);
    } catch (const std::invalid_argument &) {
        invalida = true;
    }
    exigir(invalida && Fecha(28, 2, 2026) <= Fecha(1, 3, 2026));
    exigir(!mensajes.enviarMensaje(red, 10, "ana", "Hola", fecha));
    exigir(!mensajes.enviarMensaje(red, 10, "daniel", "", fecha));
    exigir(!mensajes.enviarMensaje(red, 10, "daniel", " \t\n", fecha));
    exigir(!mensajes.enviarMensaje(red, 10, "daniel", nullptr, fecha));
    char texto[1027];
    for (int i = 0; i < 513; ++i) {
        texto[2 * i] = '\xC3';
        texto[2 * i + 1] = '\xA1';
    }
    texto[1024] = '\0';
    exigir(mensajes.enviarMensaje(red, 10, "daniel", texto, fecha));
    texto[1024] = '\xC3';
    texto[1026] = '\0';
    exigir(!mensajes.enviarMensaje(red, 10, "daniel", texto, fecha));
    exigir(!mensajes.enviarMensaje(red, 10, "daniel", "\xC3", fecha));
    exigir(mensajes.getCantidad() == 1);
    const Mensaje *original = mensajes.getMensaje(0);
    Mensaje copia(*original);
    exigir(copia.getTexto() != original->getTexto() && copia.getId() == original->getId());

    DatosPrueba datos;
    GestorArchivos base;
    AdminAmistades redReal;
    GestorMensajes historial;
    datos.cargar(base, redReal, historial);
    const char *especial = "Mensaje; con barra \\ y salto\nsegunda linea";
    exigir(historial.enviarMensaje(redReal, 1001, "juan perez", especial, Fecha::actual()));
    exigir(base.guardarEstado(redReal, historial));
    GestorArchivos recargada;
    AdminAmistades nuevaRed;
    GestorMensajes nuevoHistorial;
    datos.cargar(recargada, nuevaRed, nuevoHistorial);
    exigir(std::string(nuevoHistorial.getMensaje(5)->getTexto()) == especial);
    std::string antes = leerArchivo(datos.ruta("amistades.txt"));
    exigir(nuevaRed.envioDeSolicitud(1001, 1008));
    exigir(std::filesystem::create_directory(datos.ruta("mensajes.txt.nuevo")));
    exigir(!recargada.guardarEstado(nuevaRed, nuevoHistorial));
    exigir(nuevaRed.hayCambios() && leerArchivo(datos.ruta("amistades.txt")) == antes);
    exigir(std::filesystem::remove(datos.ruta("mensajes.txt.nuevo")));
    exigir(recargada.guardarEstado(nuevaRed, nuevoHistorial));
    exigir(!nuevaRed.hayCambios());
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
    exigir(mensajes.buscarPorRangoDeFechas(red, 20, Fecha(1, 8, 2026), Fecha(30, 9, 2026),
                                           salida) == 2);
    exigir(salida.str().find("inicio") < salida.str().find("fin"));
    exigir(!mensajes.getMensaje(1)->estaLeido());
    exigir(mensajes.buscarPorRangoDeFechas(red, 20, Fecha(30, 9, 2026), Fecha(30, 9, 2026),
                                           salida) == 1);
    exigir(mensajes.buscarPorRangoDeFechas(red, 20, Fecha(1, 10, 2026), Fecha(1, 8, 2026),
                                           salida) == -1);
    std::ostringstream fallida;
    fallida.setstate(std::ios::badbit);
    exigir(mensajes.leerNuevos(red, 20, fallida) == -1);
    exigir(!mensajes.getMensaje(1)->estaLeido());
    exigir(mensajes.leerNuevos(red, 20, salida) == 3);
    exigir(mensajes.leerNuevos(red, 20, salida) == 0);
    exigir(!mensajes.getMensaje(2)->estaLeido());
    exigir(mensajes.enviarMensaje(red, 10, "daniel", "siguiente id", Fecha(2, 10, 2026)));
    exigir(mensajes.getMensaje(5)->getId() == 6);

    DatosPrueba datos;
    GestorArchivos base;
    AdminAmistades redReal;
    GestorMensajes historial;
    datos.cargar(base, redReal, historial);
    std::ostringstream recibidos;
    exigir(historial.buscarPorRangoDeFechas(redReal, 1001, Fecha(10, 8, 2026), Fecha(5, 10, 2026),
                                            recibidos) == 4);
    exigir(historial.leerNuevos(redReal, 1001, recibidos) == 3);
    exigir(base.guardarEstado(redReal, historial));
    GestorArchivos recargada;
    AdminAmistades nuevaRed;
    GestorMensajes nuevoHistorial;
    datos.cargar(recargada, nuevaRed, nuevoHistorial);
    exigir(nuevoHistorial.leerNuevos(nuevaRed, 1001, recibidos) == 0);
    std::ofstream invalida(datos.ruta("fecha_mala.txt"));
    invalida << "1\n1;1002;1001;29;2;2026;Fecha imposible;0\n";
    invalida.close();
    exigir(!base.cargarMensajes(datos.ruta("fecha_mala.txt").c_str(), redReal, historial));
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
    red.reiniciarIteraciones();
    mensajes.reiniciarIteraciones();
    exigir(red.getIteraciones() == 0 && mensajes.getIteraciones() == 0);

    DatosPrueba datos;
    GestorArchivos base;
    Autenticacion acceso;
    AdminAmistades redReal;
    GestorMensajes historial;
    VisorPerfiles visor;
    MonitorRecursos monitor;
    Menu principal(base, acceso, redReal, visor, historial, monitor);
    {
        ConsolaPrueba consola("1001\npass123\n3\n1\nmateo herrera\n0\n5\n2\n1\njuan perez\nHola; "
                              "desde el menu\n0\n2\njuan perez\n1\n3\n3\n1\n2\n0\n0\n1\n1\n0\n");
        exigir(principal.iniciar(datos.ruta("indice.txt").c_str()));
        exigir(consola.texto().find("Memoria viva") != std::string::npos);
        exigir(consola.texto().find("solo pueden actualizarse una vez") != std::string::npos);
        exigir(consola.texto().find("/Publico/") != std::string::npos);
    }
    GestorArchivos recargada;
    AdminAmistades nuevaRed;
    GestorMensajes nuevoHistorial;
    datos.cargar(recargada, nuevaRed, nuevoHistorial);
    exigir(nuevaRed.haySolicitud(1001, 1008) && nuevoHistorial.getCantidad() == 6);
    std::ostringstream recibidos;
    exigir(nuevoHistorial.leerNuevos(nuevaRed, 1001, recibidos) == 0);
    PerfilCompleto perfil;
    exigir(recargada.leerPerfilCompleto("1002", perfil));
    exigir(perfil.getCantidadDeFotos() == 11 && perfil.getCantidadDeAlbumes() == 4);
    exigir(perfil.getFotoEnPosicion(0).getCantidadDeLikes() ==
           10); // Dar y quitar conservan el original.
    exigir(std::string(perfil.getCorreo()) == "usuario1002@udeabook.local");
    exigir(perfil.getAlbum(3).getNombre() == std::string("Vacio"));
    PerfilCompleto copia = perfil;
    copia = copia;
    copia.getFotoEnPosicion(0).setCantidadDeLikes(99);
    exigir(perfil.getFotoEnPosicion(0).getCantidadDeLikes() == 10);
    std::istringstream visita(
        "2\n3\n4\n0\n"); // Usuario ajeno: no accede a albumes de amigos ni privados.
    std::ostringstream ocultas;
    visor.mostrarCarrusel(recargada, nuevaRed, "1008", "1002", visita, ocultas);
    exigir(ocultas.str().find("/Privado/") == std::string::npos &&
           ocultas.str().find("/Amigos/") == std::string::npos);
    exigir(ocultas.str().find("Album vacio") != std::string::npos);
    {
        ConsolaPrueba consola("");
        const char *nombres[] = {"ejemplo"};
        const unsigned long long iteraciones[] = {7};
        const std::size_t bytes[] = {100};
        MonitorRecursos medicion;
        medicion.mostrarReporte(nombres, iteraciones, bytes, 1);
        std::size_t memoria = medicion.getMemoriaAcumuladaDesdeElInicio();
        medicion.mostrarReporte(nombres, iteraciones, bytes, 1);
        exigir(medicion.getMemoriaAcumuladaDesdeElInicio() == memoria);
        exigir(medicion.getIteracionesAcumuladasDesdeElInicio() == 14);
    }
    exigir(MemoriaTemporal::getBytesVivos() == 0);
}
int main(int argc, char **argv) {
    if (argc != 2)
        return 2;
    try {
        switch (argv[1][0]) {
        case '1':
            solicitudes();
            break;
        case '2':
            sugerencias();
            break;
        case '3':
            validacionMensajes();
            break;
        case '4':
            fechasYLectura();
            break;
        case '5':
            menus();
            break;
        default:
            return 2;
        }
        std::cout << "Prueba " << argv[1] << " correcta.\n";
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
