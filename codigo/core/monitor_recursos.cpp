#include "monitor_recursos.h"
#include <iostream>
std::size_t MemoriaTemporal::vivos = 0;
MemoriaTemporal::MemoriaTemporal(std::size_t cantidad) : bytes(cantidad) {
    vivos += bytes + sizeof(*this);
}
MemoriaTemporal::~MemoriaTemporal() {
    vivos -= bytes + sizeof(*this);
}
std::size_t MemoriaTemporal::getBytesVivos() {
    return vivos;
}
MonitorRecursos::MonitorRecursos() : iteracionesAcumuladasDesdeElInicio(0), memoriaActual(0) {
}
void MonitorRecursos::mostrarReporte(const char *const nombres[],
                                     const unsigned long long iteraciones[],
                                     const std::size_t memoria[], int cantidad) {
    unsigned long long total = 0;
    MemoriaTemporal locales(4 * sizeof(void *) + sizeof(cantidad) + sizeof(total));
    memoriaActual = sizeof(*this) + MemoriaTemporal::getBytesVivos() +
                    5 * sizeof(unsigned long long) + sizeof(bool) + sizeof(std::size_t) +
                    sizeof(unsigned);
    std::cout << "\n\nCONSUMO DE LA OPERACION\n";
    for (int i = 0; i < cantidad; ++i) {
        total += iteraciones[i];
        memoriaActual += memoria[i];
        std::cout << nombres[i] << ": " << iteraciones[i] << " iteraciones, " << memoria[i]
                  << " bytes\n";
    }
    iteracionesAcumuladasDesdeElInicio += total;
    std::cout << "Iteraciones de esta operacion: " << total
              << "\nMemoria viva de datos y objetos: " << memoriaActual
              << " bytes\nIteraciones acumuladas: " << iteracionesAcumuladasDesdeElInicio;
    std::cout << "\nComponentes externos usados:";
    if (getComponentes() & 1)
        std::cout << " archivos C++";
    if (getComponentes() & 2)
        std::cout << " consola C++";
    if (getComponentes() & 4)
        std::cout << " reloj C++";
    if (getComponentes() & 8)
        std::cout << " aleatorios C++";
    if (!getComponentes())
        std::cout << " ninguno";
    std::cout << "\n\n";
    reiniciarComponentes();
}
void MonitorRecursos::limpiarPantalla() {
    // Sin system(cls/clear): evita procesos de shell y conserva las salidas al redirigir.
    std::cout << "\n\n";
}
unsigned long long MonitorRecursos::getIteracionesAcumuladasDesdeElInicio() const {
    return iteracionesAcumuladasDesdeElInicio;
}
std::size_t MonitorRecursos::getMemoriaAcumuladaDesdeElInicio() const {
    return memoriaActual;
}

namespace {
unsigned long long iteracionesDeModulos[5] = {};
bool conteoPausado = false;
unsigned componentesUsados = 0;
} // namespace
void MonitorRecursos::registrarIteracion(int modulo) {
    if (!conteoPausado)
        ++iteracionesDeModulos[modulo];
}
unsigned long long MonitorRecursos::getIteracionesModulo(int modulo) {
    return iteracionesDeModulos[modulo];
}
void MonitorRecursos::reiniciarModulo(int modulo) {
    iteracionesDeModulos[modulo] = 0;
}
void MonitorRecursos::pausarConteo(bool pausa) {
    conteoPausado = pausa;
}

void MonitorRecursos::registrarComponente(unsigned componente) {
    if (!conteoPausado)
        componentesUsados |= componente;
}
unsigned MonitorRecursos::getComponentes() {
    return componentesUsados;
}
void MonitorRecursos::reiniciarComponentes() {
    componentesUsados = 0;
}
