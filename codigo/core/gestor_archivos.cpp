#include "monitor_recursos.h"
#include "gestor_archivos.h"
#include "../modulos_dinamicos/admin_amistades.h"
#include "../modulos_dinamicos/gestor_mensajes.h"
#include <fstream>
#include <filesystem>
#include <limits>
#include <stdexcept>

namespace {
bool numero(const char *texto, int &valor) {
    valor = 0;
    if (!texto || !*texto)
        return false;
    while (*texto) {
        MonitorRecursos::registrarIteracion(0);
        if (*texto < '0' || *texto > '9' ||
            valor > (std::numeric_limits<int>::max() - (*texto - '0')) / 10)
            return false;
        valor = valor * 10 + (*texto++ - '0');
    }
    return true;
}
bool iguales(const char *a, const char *b) {
    if (!a || !b)
        return a == b;
    while (*a && *a == *b) {
        MonitorRecursos::registrarIteracion(0);
        ++a;
        ++b;
    }
    return *a == *b;
}
bool linea(std::istream &archivo, char *texto, int capacidad) {
    if (!archivo.getline(texto, capacidad))
        return false;
    int n = 0;
    while (texto[n]) {
        MonitorRecursos::registrarIteracion(0);
        ++n;
    }
    if (n && texto[n - 1] == '\r')
        texto[n - 1] = '\0';
    return true;
}
bool cantidad(std::istream &archivo, int &n) {
    char texto[32];
    return linea(archivo, texto, sizeof texto) && numero(texto, n);
}
// Separacion in situ: sin asignar un arreglo nuevo por cada campo.
// Escape reversible para que ; y saltos de linea del texto no rompan registros.
int campos(char *texto, char **destinos, int maximo) {
    int n = 1;
    destinos[0] = texto;
    char *salida = texto;
    while (*texto) {
        MonitorRecursos::registrarIteracion(0);
        char c = *texto++;
        if (c == '\\') {
            c = *texto++;
            if (c == 'n')
                c = '\n';
            else if (c == 'r')
                c = '\r';
            else if (c != '\\' && c != ';')
                return -1;
            *salida++ = c;
        } else if (c == ';') {
            if (n == maximo)
                return -1;
            *salida++ = '\0';
            destinos[n++] = salida;
        } else
            *salida++ = c;
    }
    *salida = '\0';
    return n;
}
void escribirCampo(std::ostream &archivo, const char *texto) {
    for (; *texto; ++texto) {
        MonitorRecursos::registrarIteracion(0);
        if (*texto == ';' || *texto == '\\')
            archivo << '\\' << *texto;
        else if (*texto == '\n')
            archivo << "\\n";
        else if (*texto == '\r')
            archivo << "\\r";
        else
            archivo << *texto;
    }
}
bool sinRegistrosExtra(std::istream &archivo) {
    char c;
    while (archivo.get(c)) {
        MonitorRecursos::registrarIteracion(0);
        if (c != '\r' && c != '\n' && c != ' ' && c != '\t')
            return false;
    }
    return archivo.eof();
}
bool cerrar(std::ofstream &archivo) {
    archivo.flush();
    bool ok = static_cast<bool>(archivo);
    archivo.close();
    return ok && !archivo.fail();
}
// Todos los temporales son salidas finales pendientes de confirmar, nunca area
// de procesamiento. Ante un fallo se conservan/restauran los archivos anteriores.
bool confirmarArchivos(const char *const *rutas, int cantidadArchivos) {
    namespace fs = std::filesystem;
    std::error_code error;
    int respaldados = 0, instalados = 0;
    try {
        for (; respaldados < cantidadArchivos; ++respaldados) {
            MonitorRecursos::registrarIteracion(0);
            fs::path ruta(rutas[respaldados]);
            fs::path respaldo = ruta;
            respaldo += ".anterior";
            if (fs::exists(respaldo))
                throw std::runtime_error("Respaldo pendiente de recuperar");
            fs::rename(ruta, respaldo);
        }
        for (; instalados < cantidadArchivos; ++instalados) {
            MonitorRecursos::registrarIteracion(0);
            fs::path ruta(rutas[instalados]);
            fs::path nuevo = ruta;
            nuevo += ".nuevo";
            fs::rename(nuevo, ruta);
        }
    } catch (...) {
        for (int i = 0; i < instalados; ++i) {
            MonitorRecursos::registrarIteracion(0);
            fs::remove(rutas[i], error);
        }
        for (int i = 0; i < respaldados; ++i) {
            MonitorRecursos::registrarIteracion(0);
            fs::path respaldo(rutas[i]);
            respaldo += ".anterior";
            fs::rename(respaldo, rutas[i], error);
        }
        return false;
    }
    for (int i = 0; i < cantidadArchivos; ++i) {
        MonitorRecursos::registrarIteracion(0);
        fs::path respaldo(rutas[i]);
        respaldo += ".anterior";
        fs::remove(respaldo, error);
    }
    return true;
}
} // namespace
GestorArchivos::GestorArchivos() : arregloUsuariosLigeros(nullptr), totalUsuarios(0), rutaBase{} {
}
GestorArchivos::~GestorArchivos() {
    delete[] arregloUsuariosLigeros;
}
bool GestorArchivos::construirRuta(const char *archivo, char *destino,
                                   std::size_t capacidad) const {
    if (!archivo || !destino || !capacidad)
        return false;
    std::size_t i = 0;
    for (const char *p = rutaBase; *p; ++p) {
        MonitorRecursos::registrarIteracion(0);

        if (i + 1 >= capacidad)
            return false;
        destino[i++] = *p;
    }
    for (; *archivo; ++archivo) {
        MonitorRecursos::registrarIteracion(0);

        if (i + 1 >= capacidad)
            return false;
        destino[i++] = *archivo;
    }
    destino[i] = '\0';
    return true;
}
bool GestorArchivos::cargarIndiceUsuarios(const char *rutaArchivo) {
    if (!rutaArchivo)
        return false;
    MonitorRecursos::registrarComponente(1);
    std::ifstream archivo(rutaArchivo);
    int n = 0;
    if (!cantidad(archivo, n) || n == 0)
        return false;
    UsuarioLigero *nuevos = new UsuarioLigero[n];
    try {
        char texto[4096];
        char *datos[3];
        for (int i = 0; i < n; ++i) {
            MonitorRecursos::registrarIteracion(0);

            int id = 0;
            if (!linea(archivo, texto, sizeof texto) || campos(texto, datos, 3) != 3 ||
                !numero(datos[0], id) || id <= 0 || !*datos[1] || !*datos[2]) {
                delete[] nuevos;
                return false;
            }
            for (int j = 0; j < i; ++j) {
                MonitorRecursos::registrarIteracion(0);

                int previo = 0;
                numero(nuevos[j].getIdUsuario(), previo);
                if (id == previo || iguales(datos[2], nuevos[j].getNombreUsuario())) {
                    delete[] nuevos;
                    return false;
                }
            }
            nuevos[i].setIdUsuario(datos[0]);
            nuevos[i].setContrasena(datos[1]);
            nuevos[i].setNombreUsuario(datos[2]);
        }
        if (!sinRegistrosExtra(archivo)) {
            delete[] nuevos;
            return false;
        }
        int ultimo = -1, largo = 0;
        for (; rutaArchivo[largo]; ++largo) {
            MonitorRecursos::registrarIteracion(0);
            if (rutaArchivo[largo] == '/' || rutaArchivo[largo] == '\\')
                ultimo = largo;
        }
        if (ultimo + 1 >= static_cast<int>(sizeof rutaBase)) {
            delete[] nuevos;
            return false;
        }
        for (int i = 0; i <= ultimo; ++i) {
            MonitorRecursos::registrarIteracion(0);
            rutaBase[i] = rutaArchivo[i];
        }
        rutaBase[ultimo + 1] = '\0';
    } catch (...) {
        delete[] nuevos;
        throw;
    }
    delete[] arregloUsuariosLigeros;
    arregloUsuariosLigeros = nuevos;
    totalUsuarios = n;
    return true;
}
int GestorArchivos::getTotalUsuarios() const {
    return totalUsuarios;
}
const UsuarioLigero &GestorArchivos::getUsuarioLigero(int i) const {
    if (i < 0 || i >= totalUsuarios)
        throw std::out_of_range("Usuario inexistente");
    return arregloUsuariosLigeros[i];
}
bool GestorArchivos::leerPerfilCompleto(const char *idUsuario, PerfilCompleto &destino) const {
    char ruta[1200];
    if (!construirRutaPerfil(idUsuario, ruta, sizeof ruta))
        return false;
    MonitorRecursos::registrarComponente(1);
    std::ifstream archivo(ruta);
    char texto[4096];
    int n = 0;
    PerfilCompleto perfil;
    if (!linea(archivo, texto, sizeof texto) || !*texto)
        return false;
    perfil.setRutaCarpetaDeFotos(texto);
    if (!cantidad(archivo, n))
        return false;
    perfil.inicializarFotos(n);
    try {
        for (int i = 0; i < n; ++i) {
            MonitorRecursos::registrarIteracion(0);

            char *datos[8];
            int likes = 0, privacidad = 0, dia = 1, mes = 1, anio = 2000, bytes = 0;
            if (!linea(archivo, texto, sizeof texto))
                return false;
            int total = campos(texto, datos, 8);
            const char *nombre = nullptr;
            const char *album = nullptr;
            if (total == 3) {
                nombre = datos[0];
                if (!numero(datos[1], likes) || !numero(datos[2], privacidad) || privacidad > 1)
                    return false;
                album = privacidad ? "Publico" : "Amigos";
            } else if (total == 8) {
                album = datos[0];
                nombre = datos[1];
                if (!numero(datos[2], likes) || !numero(datos[3], privacidad) || privacidad > 2 ||
                    !numero(datos[4], dia) || !numero(datos[5], mes) || !numero(datos[6], anio) ||
                    !numero(datos[7], bytes))
                    return false;
            } else
                return false;
            if (!*nombre || !*album)
                return false;
            int indice = perfil.incorporarAlbum(album, privacidad);
            for (int j = 0; j < i; ++j) {
                MonitorRecursos::registrarIteracion(0);

                const FotoConLikes &previa = perfil.getFotoEnPosicion(j);
                if (previa.getIndiceAlbum() == indice &&
                    iguales(previa.getNombreDeArchivo(), nombre))
                    return false;
            }
            FotoConLikes &foto = perfil.getFotoEnPosicion(i);
            foto.setNombreDeArchivo(nombre);
            foto.setCantidadDeLikes(likes);
            foto.setIndiceAlbum(indice);
            foto.setFechaCaptura(Fecha(dia, mes, anio));
            foto.setTamano(static_cast<std::size_t>(bytes));
        }
    } catch (const std::invalid_argument &) {
        return false;
    }
    // Registros opcionales conservan albumes vacios y metadatos sin cambiar las filas de fotos.
    bool tieneDatos = false;
    try {
        while (linea(archivo, texto, sizeof texto)) {
            MonitorRecursos::registrarIteracion(0);
            if (!*texto)
                continue;
            char *datos[10];
            int total = campos(texto, datos, 10), privacidad = 0;
            if (total == 3 && iguales(datos[0], "ALBUM") && numero(datos[2], privacidad) &&
                privacidad <= 2) {
                perfil.incorporarAlbum(datos[1], privacidad);
            } else if (total == 10 && iguales(datos[0], "USUARIO") && !tieneDatos) {
                int fechas[6];
                for (int j = 0; j < 6; ++j) {
                    MonitorRecursos::registrarIteracion(0);
                    if (!numero(datos[j + 4], fechas[j]))
                        return false;
                }
                perfil.setDatosUsuario(datos[1], datos[2], datos[3],
                                       Fecha(fechas[0], fechas[1], fechas[2]),
                                       Fecha(fechas[3], fechas[4], fechas[5]));
                tieneDatos = true;
            } else
                return false;
        }
    } catch (const std::invalid_argument &) {
        return false;
    }
    if (!archivo.eof())
        return false;
    destino = perfil;
    return true;
}
bool GestorArchivos::guardarCambioDeLike(const char *id, const char *nombre, int likes,
                                         const char *album) const {
    if (!nombre || likes < 0)
        return false;
    PerfilCompleto perfil;
    if (!leerPerfilCompleto(id, perfil))
        return false;
    int encontrada = -1;
    for (int i = 0; i < perfil.getCantidadDeFotos(); ++i) {
        MonitorRecursos::registrarIteracion(0);

        const FotoConLikes &foto = perfil.getFotoEnPosicion(i);
        if (iguales(nombre, foto.getNombreDeArchivo()) &&
            (!album || iguales(album, perfil.getAlbum(foto.getIndiceAlbum()).getNombre()))) {
            if (encontrada >= 0)
                return false; // Nombre ambiguo entre albumes.
            encontrada = i;
        }
    }
    if (encontrada < 0)
        return false;
    perfil.getFotoEnPosicion(encontrada).setCantidadDeLikes(likes);
    char ruta[1200], nueva[1210];
    if (!construirRutaPerfil(id, ruta, sizeof ruta))
        return false;
    int pos = 0;
    pos = 0;
    while (ruta[pos]) {
        MonitorRecursos::registrarIteracion(0);
        nueva[pos] = ruta[pos];
        ++pos;
    }
    for (const char *p = ".nuevo"; *p; ++p) {
        MonitorRecursos::registrarIteracion(0);
        nueva[pos++] = *p;
    }
    nueva[pos] = '\0';
    MonitorRecursos::registrarComponente(1);
    std::ofstream archivo(nueva);
    if (!archivo)
        return false;
    archivo << perfil.getRutaCarpetaDeFotos() << '\n' << perfil.getCantidadDeFotos() << '\n';
    for (int i = 0; i < perfil.getCantidadDeFotos(); ++i) {
        MonitorRecursos::registrarIteracion(0);

        const FotoConLikes &f = perfil.getFotoEnPosicion(i);
        escribirCampo(archivo, perfil.getAlbum(f.getIndiceAlbum()).getNombre());
        archivo << ';';
        escribirCampo(archivo, f.getNombreDeArchivo());
        archivo << ';' << f.getCantidadDeLikes() << ';'
                << perfil.getAlbum(f.getIndiceAlbum()).getPrivacidad() << ';'
                << f.getFechaCaptura().getDia() << ';' << f.getFechaCaptura().getMes() << ';'
                << f.getFechaCaptura().getAnio() << ';' << f.getTamano() << '\n';
    }
    for (int i = 0; i < perfil.getCantidadDeAlbumes(); ++i) {
        MonitorRecursos::registrarIteracion(0);
        archivo << "ALBUM;";
        escribirCampo(archivo, perfil.getAlbum(i).getNombre());
        archivo << ';' << perfil.getAlbum(i).getPrivacidad() << '\n';
    }
    if (perfil.getCorreo()) {
        archivo << "USUARIO;";
        escribirCampo(archivo, perfil.getCorreo());
        archivo << ';';
        escribirCampo(archivo, perfil.getCiudad());
        archivo << ';';
        escribirCampo(archivo, perfil.getPais());
        archivo << ';' << perfil.getNacimiento().getDia() << ';' << perfil.getNacimiento().getMes()
                << ';' << perfil.getNacimiento().getAnio() << ';'
                << perfil.getInscripcion().getDia() << ';' << perfil.getInscripcion().getMes()
                << ';' << perfil.getInscripcion().getAnio() << '\n';
    }
    const char *rutas[] = {ruta};
    return cerrar(archivo) && confirmarArchivos(rutas, 1);
}
bool GestorArchivos::cargarAmistades(const char *ruta, AdminAmistades &red) const {
    MonitorRecursos::registrarComponente(1);
    std::ifstream archivo(ruta);
    int n = 0;
    if (!cantidad(archivo, n))
        return false;
    for (int i = 0; i < n; ++i) {
        MonitorRecursos::registrarIteracion(0);

        char texto[64];
        char *datos[2];
        int a = 0, b = 0;
        if (!linea(archivo, texto, sizeof texto) || campos(texto, datos, 2) != 2 ||
            !numero(datos[0], a) || !numero(datos[1], b) || red.sonAmigos(a, b) ||
            !red.cargarAmistad(a, b))
            return false;
    }
    return sinRegistrosExtra(archivo);
}
bool GestorArchivos::cargarSolicitudes(const char *ruta, AdminAmistades &red) const {
    MonitorRecursos::registrarComponente(1);
    std::ifstream archivo(ruta);
    int n = 0;
    if (!cantidad(archivo, n))
        return false;
    for (int i = 0; i < n; ++i) {
        MonitorRecursos::registrarIteracion(0);

        char texto[64];
        char *datos[2];
        int a = 0, b = 0;
        if (!linea(archivo, texto, sizeof texto) || campos(texto, datos, 2) != 2 ||
            !numero(datos[0], a) || !numero(datos[1], b) || !red.cargarSolicitud(a, b))
            return false;
    }
    return sinRegistrosExtra(archivo);
}
bool GestorArchivos::cargarMensajes(const char *ruta, const AdminAmistades &red,
                                    GestorMensajes &mensajes) const {
    MonitorRecursos::registrarComponente(1);
    std::ifstream archivo(ruta);
    int n = 0;
    if (!cantidad(archivo, n))
        return false;
    for (int i = 0; i < n; ++i) {
        MonitorRecursos::registrarIteracion(0);

        char texto[8192];
        char *datos[8];
        int valores[6], leido = 0;
        if (!linea(archivo, texto, sizeof texto) || campos(texto, datos, 8) != 8)
            return false;
        for (int j = 0; j < 6; ++j) {
            MonitorRecursos::registrarIteracion(0);
            if (!numero(datos[j], valores[j]))
                return false;
        }
        if (!numero(datos[7], leido) || leido > 1)
            return false;
        try {
            if (!mensajes.cargarMensaje(red, valores[0], valores[1], valores[2],
                                        Fecha(valores[3], valores[4], valores[5]), datos[6],
                                        leido == 1))
                return false;
        } catch (const std::invalid_argument &) {
            return false;
        }
    }
    return sinRegistrosExtra(archivo);
}
bool GestorArchivos::guardarEstado(AdminAmistades &red, GestorMensajes &mensajes) const {
    if (!red.hayCambios() && !mensajes.hayCambios())
        return true;
    const char *archivos[] = {"amistades.txt", "solicitudes.txt", "mensajes.txt"};
    char rutas[3][1200], nuevas[3][1210];
    for (int i = 0; i < 3; ++i) {
        MonitorRecursos::registrarIteracion(0);
        if (!construirRuta(archivos[i], rutas[i], sizeof rutas[i]))
            return false;
        int pos = 0;
        while (rutas[i][pos]) {
            MonitorRecursos::registrarIteracion(0);
            nuevas[i][pos] = rutas[i][pos];
            ++pos;
        }
        for (const char *p = ".nuevo"; *p; ++p) {
            MonitorRecursos::registrarIteracion(0);
            nuevas[i][pos++] = *p;
        }
        nuevas[i][pos] = '\0';
    }
    MonitorRecursos::registrarComponente(1);
    std::ofstream amigos(nuevas[0]), solicitudes(nuevas[1]), historial(nuevas[2]);
    if (!amigos || !solicitudes || !historial)
        return false;
    int totalAmigos = 0, totalSolicitudes = 0;
    for (int i = 0; i < red.getCantidadUsuarios(); ++i) {
        MonitorRecursos::registrarIteracion(0);
        for (int j = 0; j < red.getCantidadUsuarios(); ++j) {
            MonitorRecursos::registrarIteracion(0);

            if (i < j && red.getRelacion(i, j) == 1)
                ++totalAmigos;
            if (red.getRelacion(i, j) == 2)
                ++totalSolicitudes;
        }
    }
    amigos << totalAmigos << '\n';
    solicitudes << totalSolicitudes << '\n';
    for (int i = 0; i < red.getCantidadUsuarios(); ++i) {
        MonitorRecursos::registrarIteracion(0);
        for (int j = 0; j < red.getCantidadUsuarios(); ++j) {
            MonitorRecursos::registrarIteracion(0);

            int a = red.getIdEnPosicion(i), b = red.getIdEnPosicion(j);
            if (i < j && red.getRelacion(i, j) == 1)
                amigos << a << ';' << b << '\n';
            if (red.getRelacion(i, j) == 2)
                solicitudes << a << ';' << b << '\n';
        }
    }
    historial << mensajes.getCantidad() << '\n';
    for (int i = 0; i < mensajes.getCantidad(); ++i) {
        MonitorRecursos::registrarIteracion(0);

        const Mensaje &m = *mensajes.getMensaje(i);
        historial << m.getId() << ';' << m.getEmisor() << ';' << m.getReceptor() << ';'
                  << m.getFecha().getDia() << ';' << m.getFecha().getMes() << ';'
                  << m.getFecha().getAnio() << ';';
        escribirCampo(historial, m.getTexto());
        historial << ';' << (m.estaLeido() ? 1 : 0) << '\n';
    }
    bool okAmigos = cerrar(amigos), okSolicitudes = cerrar(solicitudes),
         okHistorial = cerrar(historial);
    const char *destinos[] = {rutas[0], rutas[1], rutas[2]};
    if (!okAmigos || !okSolicitudes || !okHistorial || !confirmarArchivos(destinos, 3))
        return false;
    red.confirmarGuardado();
    mensajes.confirmarGuardado();
    return true;
}
void GestorArchivos::reiniciarIteraciones() const {
    MonitorRecursos::reiniciarModulo(0);
}
unsigned long long GestorArchivos::getIteraciones() const {
    return MonitorRecursos::getIteracionesModulo(0);
}
std::size_t GestorArchivos::getMemoriaReservada() const {
    std::size_t total = sizeof(*this);
    for (int i = 0; i < totalUsuarios; ++i) {
        MonitorRecursos::registrarIteracion(0);
        total += arregloUsuariosLigeros[i].getMemoriaReservada();
    }
    return total;
}

bool GestorArchivos::construirRutaPerfil(const char *id, char *destino,
                                         std::size_t capacidad) const {
    int valor = 0;
    if (!numero(id, valor) || valor <= 0)
        return false;
    char relativo[64];
    int pos = 0;
    for (const char *p = "perfiles/"; *p; ++p) {
        MonitorRecursos::registrarIteracion(0);
        relativo[pos++] = *p;
    }
    for (const char *p = id; *p; ++p) {
        MonitorRecursos::registrarIteracion(0);
        if (pos >= 55)
            return false;
        relativo[pos++] = *p;
    }
    for (const char *p = ".txt"; *p; ++p) {
        MonitorRecursos::registrarIteracion(0);
        relativo[pos++] = *p;
    }
    relativo[pos] = '\0';
    return construirRuta(relativo, destino, capacidad);
}
bool GestorArchivos::leerRutaFotoPerfil(const char *id, char *destino,
                                        std::size_t capacidad) const {
    char ruta[1200], raiz[4096];
    if (!destino || !capacidad || !construirRutaPerfil(id, ruta, sizeof ruta))
        return false;
    MonitorRecursos::registrarComponente(1);
    std::ifstream archivo(ruta);
    if (!linea(archivo, raiz, sizeof raiz) || !*raiz)
        return false;
    std::size_t pos = 0;
    for (const char *p = raiz; *p; ++p) {
        MonitorRecursos::registrarIteracion(0);
        if (pos + 1 >= capacidad)
            return false;
        destino[pos++] = *p;
    }
    for (const char *p = "/profile.png"; *p; ++p) {
        MonitorRecursos::registrarIteracion(0);
        if (pos + 1 >= capacidad)
            return false;
        destino[pos++] = *p;
    }
    destino[pos] = '\0';
    return true;
}
