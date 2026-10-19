#include "visor_perfiles.h"
#include "../core/monitor_recursos.h"
#include <istream>
#include <ostream>
#include <limits>

namespace {
bool iguales(const char *a, const char *b) {
    if (!a || !b)
        return false;
    while (*a && *a == *b) {
        MonitorRecursos::registrarIteracion(3);
        ++a;
        ++b;
    }
    return *a == *b;
}
int numero(const char *texto) {
    int n = 0;
    if (!texto)
        return 0;
    while (*texto) {
        MonitorRecursos::registrarIteracion(3);
        if (*texto < '0' || *texto > '9' ||
            n > (std::numeric_limits<int>::max() - (*texto - '0')) / 10)
            return 0;
        n = n * 10 + (*texto++ - '0');
    }
    return n;
}
bool leerOpcion(std::istream &entrada, int &opcion) {
    int nueva;
    if (entrada >> nueva) {
        entrada.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        opcion = nueva;
        return true;
    }
    if (!entrada.eof()) {
        entrada.clear();
        entrada.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return false;
}
bool visible(int privacidad, bool propio, bool amigo) {
    return propio || privacidad == 1 || (privacidad == 0 && amigo);
}
} // namespace
VisorPerfiles::VisorPerfiles() {
}
bool VisorPerfiles::buscarPerfilPorNombre(const GestorArchivos &base, const char *nombre,
                                          char *destino, std::size_t capacidad) const {
    if (!destino || !capacidad)
        return false;
    destino[0] = '\0';
    for (int i = 0; i < base.getTotalUsuarios(); ++i) {
        MonitorRecursos::registrarIteracion(3);

        const UsuarioLigero &u = base.getUsuarioLigero(i);
        if (!iguales(nombre, u.getNombreUsuario()))
            continue;
        std::size_t n = 0;
        while (u.getIdUsuario()[n]) {
            MonitorRecursos::registrarIteracion(3);
            ++n;
        }
        if (n >= capacidad)
            return false;
        for (std::size_t j = 0; j <= n; ++j) {
            MonitorRecursos::registrarIteracion(3);
            destino[j] = u.getIdUsuario()[j];
        }
        return true;
    }
    return false;
}
void VisorPerfiles::mostrarCarrusel(const GestorArchivos &base, const AdminAmistades &red,
                                    const char *idPropio, const char *idPerfil,
                                    std::istream &entrada, std::ostream &salida,
                                    void (*alTerminar)(void *), void *contexto) const {
    MonitorRecursos::registrarComponente(2);
    if (red.buscarUsuario(numero(idPropio)) < 0 || red.buscarUsuario(numero(idPerfil)) < 0) {
        salida << "Usuario inexistente.\n";
        return;
    }
    PerfilCompleto perfil;
    if (!base.leerPerfilCompleto(idPerfil, perfil)) {
        salida << "Perfil no disponible o invalido.\n";
        return;
    }
    bool propio = iguales(idPropio, idPerfil),
         amigo = red.sonAmigos(numero(idPropio), numero(idPerfil));
    int *indices = new int[perfil.getCantidadDeFotos()];
    MemoriaTemporal memoria(perfil.getMemoriaReservada() +
                            static_cast<std::size_t>(perfil.getCantidadDeFotos()) * sizeof(int) +
                            11 * sizeof(void *) + 5 * sizeof(int) + 3 * sizeof(bool));
    // El arreglo de indices se comparte entre albumes; el perfil se carga una sola vez.
    try {
        while (entrada && salida) {
            MonitorRecursos::registrarIteracion(3);
            salida << "\nALBUMES VISIBLES\n";
            int visibles = 0;
            for (int i = 0; i < perfil.getCantidadDeAlbumes(); ++i) {
                MonitorRecursos::registrarIteracion(3);

                const Album &album = perfil.getAlbum(i);
                if (!visible(album.getPrivacidad(), propio, amigo))
                    continue;
                salida << i + 1 << ". " << album.getNombre() << '\n';
                ++visibles;
            }
            if (!visibles) {
                salida << "No hay albumes visibles.\n";
                break;
            }
            salida << "Album (0 para volver): ";
            int elegido = -1;
            if (!leerOpcion(entrada, elegido)) {
                salida << "Ingresa un numero.\n";
                continue;
            }
            if (!elegido)
                break;
            if (elegido < 1 || elegido > perfil.getCantidadDeAlbumes() ||
                !visible(perfil.getAlbum(elegido - 1).getPrivacidad(), propio, amigo)) {
                salida << "Album no disponible.\n";
                continue;
            }
            int cantidad = 0;
            for (int i = 0; i < perfil.getCantidadDeFotos(); ++i) {
                MonitorRecursos::registrarIteracion(3);

                if (perfil.getFotoEnPosicion(i).getIndiceAlbum() == elegido - 1)
                    indices[cantidad++] = i;
            }
            if (!cantidad) {
                salida << "Album vacio.\n";
                continue;
            }
            int actual = 0;
            bool dentro = true;
            while (dentro && entrada && salida) {
                MonitorRecursos::registrarIteracion(3);

                FotoConLikes &foto = perfil.getFotoEnPosicion(indices[actual]);
                salida << "\n"
                       << perfil.getRutaCarpetaDeFotos() << '/'
                       << perfil.getAlbum(elegido - 1).getNombre() << '/'
                       << foto.getNombreDeArchivo() << " | likes: " << foto.getCantidadDeLikes()
                       << '\n';
                salida
                    << "0. Salir del album\n1. Siguiente\n2. Previa\n3. Dar/quitar like\nOpcion: ";
                int accion = -1;
                if (!leerOpcion(entrada, accion)) {
                    salida << "Ingresa un numero.\n";
                    continue;
                }
                if (!accion)
                    dentro = false;
                else if (accion == 1)
                    actual = (actual + 1) % cantidad;
                else if (accion == 2)
                    actual = (actual + cantidad - 1) % cantidad;
                else if (accion == 3) {
                    int likes = foto.getCantidadDeLikes();
                    if (!foto.getLikeEnSesion() && likes == std::numeric_limits<int>::max()) {
                        salida << "Contador de likes lleno.\n";
                    } else {
                        int nuevo = likes + (foto.getLikeEnSesion() ? -1 : 1);
                        if (base.guardarCambioDeLike(idPerfil, foto.getNombreDeArchivo(), nuevo,
                                                     perfil.getAlbum(elegido - 1).getNombre())) {
                            foto.setCantidadDeLikes(nuevo);
                            foto.setLikeEnSesion(!foto.getLikeEnSesion());
                            salida << "Like actualizado.\n";
                        } else
                            salida << "No se pudo guardar el like; el contador se conserva.\n";
                    }
                } else
                    salida << "Opcion invalida.\n";
                if (accion && alTerminar)
                    alTerminar(contexto);
            }
        }
    } catch (...) {
        delete[] indices;
        throw;
    }
    delete[] indices;
}
void VisorPerfiles::reiniciarIteraciones() const {
    MonitorRecursos::reiniciarModulo(3);
}
unsigned long long VisorPerfiles::getIteraciones() const {
    return MonitorRecursos::getIteracionesModulo(3);
}
std::size_t VisorPerfiles::getMemoriaReservada() const {
    return sizeof(*this);
}
