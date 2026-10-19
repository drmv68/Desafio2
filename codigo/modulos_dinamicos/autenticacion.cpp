#include "autenticacion.h"
#include "../core/monitor_recursos.h"
#include <iostream>
#include <limits>

namespace {
bool iguales(const char *a, const char *b) {
    if (!a || !b)
        return false;
    while (*a && *a == *b) {
        MonitorRecursos::registrarIteracion(2);
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
        MonitorRecursos::registrarIteracion(2);
        if (*texto < '0' || *texto > '9' ||
            n > (std::numeric_limits<int>::max() - (*texto - '0')) / 10)
            return 0;
        n = n * 10 + (*texto++ - '0');
    }
    return n;
}
} // namespace
Autenticacion::Autenticacion()
    : idDelUsuarioActivo(nullptr), amigosMostrados{}, cantidadMostrada(0),
      actualizacionUsada(false), generador(std::random_device{}()) {
    MonitorRecursos::registrarComponente(8);
}
Autenticacion::~Autenticacion() {
    delete[] idDelUsuarioActivo;
}
bool Autenticacion::iniciarSesion(const GestorArchivos &base) {
    return iniciarSesion(base, std::cin, std::cout);
}
bool Autenticacion::iniciarSesion(const GestorArchivos &base, std::istream &entrada,
                                  std::ostream &salida) {
    delete[] idDelUsuarioActivo;
    idDelUsuarioActivo = nullptr;
    cantidadMostrada = 0;
    actualizacionUsada = false;
    MonitorRecursos::registrarComponente(2);
    char id[100], clave[100];
    salida << "Id de usuario: ";
    if (!entrada.getline(id, sizeof id))
        return false;
    salida << "Contrasena: ";
    if (!entrada.getline(clave, sizeof clave))
        return false;
    for (int i = 0; i < base.getTotalUsuarios(); ++i) {
        MonitorRecursos::registrarIteracion(2);

        const UsuarioLigero &u = base.getUsuarioLigero(i);
        if (!iguales(u.getIdUsuario(), id) || !iguales(u.getContrasena(), clave))
            continue;
        int n = 0;
        while (id[n]) {
            MonitorRecursos::registrarIteracion(2);
            ++n;
        }
        idDelUsuarioActivo = new char[n + 1];
        for (int j = 0; j <= n; ++j) {
            MonitorRecursos::registrarIteracion(2);
            idDelUsuarioActivo[j] = id[j];
        }
        salida << "Bienvenido, " << u.getNombreUsuario() << ".\n";
        return true;
    }
    salida << "Credenciales incorrectas.\n";
    return false;
}
const char *Autenticacion::getIdDelUsuarioActivo() const {
    return idDelUsuarioActivo;
}
bool Autenticacion::mostrarMejoresAmigos(const GestorArchivos &base, const AdminAmistades &red,
                                         bool actualizar, std::ostream &salida) {
    if (!idDelUsuarioActivo)
        return false;
    if (actualizar && actualizacionUsada) {
        salida << "Las fotos de amigos solo pueden actualizarse una vez por sesion.\n";
        return false;
    }
    if (actualizar)
        actualizacionUsada = true;
    int activo = numero(idDelUsuarioActivo), n = red.getCantidadUsuarios();
    int *candidatos = new int[n];
    int *pesos = nullptr;
    try {
        pesos = new int[n];
    } catch (...) {
        delete[] candidatos;
        throw;
    }
    MemoriaTemporal memoria(sizeof(int) * static_cast<std::size_t>(n) * 2);
    int total = 0, nuevos = 0;
    for (int i = 0; i < n; ++i) {
        MonitorRecursos::registrarIteracion(2);

        int id = red.getIdEnPosicion(i);
        if (!red.sonAmigos(activo, id))
            continue;
        bool anterior = false;
        for (int j = 0; j < cantidadMostrada; ++j) {
            MonitorRecursos::registrarIteracion(2);
            if (id == amigosMostrados[j])
                anterior = true;
        }
        candidatos[total] = id;
        int comunes = red.contarAmigosComunes(activo, id);
        pesos[total] = comunes <= 10 ? 2 : comunes <= 20 ? 5 : 7;
        // En la actualizacion se prefieren amigos que no estaban en el grupo previo.
        if (!anterior)
            ++nuevos;
        ++total;
    }
    if (actualizar && nuevos == 0) {
        salida << "No hay otros amigos para formar un grupo diferente.\n";
        delete[] candidatos;
        delete[] pesos;
        return false;
    }
    int anteriores[3];
    int cantidadAnterior = cantidadMostrada;
    for (int i = 0; i < cantidadAnterior; ++i) {
        MonitorRecursos::registrarIteracion(2);
        anteriores[i] = amigosMostrados[i];
    }
    cantidadMostrada = 0;
    int objetivo = total < 3 ? total : 3;
    for (int seleccion = 0; seleccion < objetivo; ++seleccion) {
        MonitorRecursos::registrarIteracion(2);

        bool preferirNuevos = actualizar && nuevos > 0;
        int suma = 0;
        for (int i = 0; i < total; ++i) {
            MonitorRecursos::registrarIteracion(2);

            bool anterior = false;
            for (int j = 0; j < cantidadAnterior; ++j) {
                MonitorRecursos::registrarIteracion(2);
                if (candidatos[i] == anteriores[j])
                    anterior = true;
            }
            if (pesos[i] && (!preferirNuevos || !anterior))
                suma += pesos[i];
        }
        MonitorRecursos::registrarComponente(8);
        int sorteo = std::uniform_int_distribution<int>(1, suma)(generador);
        for (int i = 0; i < total; ++i) {
            MonitorRecursos::registrarIteracion(2);

            bool anterior = false;
            for (int j = 0; j < cantidadAnterior; ++j) {
                MonitorRecursos::registrarIteracion(2);
                if (candidatos[i] == anteriores[j])
                    anterior = true;
            }
            if (!pesos[i] || (preferirNuevos && anterior))
                continue;
            sorteo -= pesos[i];
            if (sorteo <= 0) {
                amigosMostrados[cantidadMostrada++] = candidatos[i];
                pesos[i] = 0;
                if (!anterior)
                    --nuevos;
                break;
            }
        }
    }
    delete[] candidatos;
    delete[] pesos;
    salida << "\nFotos de tus amigos (seleccion aleatoria ponderada):\n";
    for (int i = 0; i < cantidadMostrada; ++i) {
        MonitorRecursos::registrarIteracion(2);

        int id = amigosMostrados[i];
        for (int j = 0; j < base.getTotalUsuarios(); ++j) {
            MonitorRecursos::registrarIteracion(2);

            const UsuarioLigero &u = base.getUsuarioLigero(j);
            if (numero(u.getIdUsuario()) != id)
                continue;
            char rutaFoto[4200];
            salida << red.getNombre(id) << " | ";
            if (base.leerRutaFotoPerfil(u.getIdUsuario(), rutaFoto, sizeof rutaFoto))
                salida << rutaFoto << '\n';
            else
                salida << "Perfil sin ruta disponible.\n";
            break;
        }
    }
    if (!cantidadMostrada)
        salida << "Todavia no tienes amigos confirmados.\n";
    return true;
}
int Autenticacion::getAmigoMostrado(int i) const {
    return i >= 0 && i < cantidadMostrada ? amigosMostrados[i] : -1;
}
void Autenticacion::reiniciarIteraciones() const {
    MonitorRecursos::reiniciarModulo(2);
}
unsigned long long Autenticacion::getIteraciones() const {
    return MonitorRecursos::getIteracionesModulo(2);
}
std::size_t Autenticacion::getMemoriaReservada() const {
    std::size_t n = 0;
    if (idDelUsuarioActivo) {
        while (idDelUsuarioActivo[n]) {
            MonitorRecursos::registrarIteracion(2);
            ++n;
        }
        ++n;
    }
    return sizeof(*this) + n;
}
