#include "monitor_recursos.h"
#include <iostream>
#include <cstdlib>

using namespace std;

MonitorRecursos::MonitorRecursos() {
    iteracionesAcumuladasDesdeElInicio = 0;
    memoriaAcumuladaDesdeElInicio = 0;
}

void MonitorRecursos::mostrarReporte(const char* const nombresDeModulos[],
                                      const unsigned long long iteracionesPorModulo[],
                                      const std::size_t memoriaPorModulo[],
                                      int cantidadDeModulosReportados) {

    cout << "\n\n";
    cout << "================================================" << endl;
    cout << "    CONSUMO DE RECURSOS DE LA ULTIMA OPERACION" << endl;
    cout << "================================================" << endl;
    cout << endl;

    int posicionActual = 0;
    while (posicionActual < cantidadDeModulosReportados) {

        cout << "  Modulo: " << nombresDeModulos[posicionActual] << endl;
        cout << "    Iteraciones dadas : " << iteracionesPorModulo[posicionActual] << endl;
        cout << "    Memoria reservada : " << memoriaPorModulo[posicionActual] << " bytes" << endl;
        cout << endl;

        iteracionesAcumuladasDesdeElInicio += iteracionesPorModulo[posicionActual];
        memoriaAcumuladaDesdeElInicio += memoriaPorModulo[posicionActual];

        posicionActual = posicionActual + 1;
    }

    cout << "  -- Acumulado desde que arranco el programa --" << endl;
    cout << "    Iteraciones totales : " << iteracionesAcumuladasDesdeElInicio << endl;
    cout << "    Memoria total       : " << memoriaAcumuladaDesdeElInicio << " bytes" << endl;
    cout << "================================================" << endl;
    cout << "\n\n";
}

void MonitorRecursos::limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

unsigned long long MonitorRecursos::getIteracionesAcumuladasDesdeElInicio() const {
    return iteracionesAcumuladasDesdeElInicio;
}

std::size_t MonitorRecursos::getMemoriaAcumuladaDesdeElInicio() const {
    return memoriaAcumuladaDesdeElInicio;
}
