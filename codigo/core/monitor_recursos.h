#ifndef MONITOR_RECURSOS_H
#define MONITOR_RECURSOS_H

#include <cstddef>

// ============================================================
// core/monitor_recursos.h
// Modulo 7 del analisis: "Medicion del consumo de recursos".
//
// Ya no es "el espia infiltrado dentro de cada ciclo": Admin
// Amistades y GestorMensajes ya traen su propio contador interno
// (getIteraciones(), getMemoriaReservada(), reiniciarIteraciones()).
// MonitorRecursos ahora es la clase "preguntona": al terminar
// cada opcion del menu, main.cpp le entrega los numeros que ya
// juntó de cada modulo, y esta clase solo imprime el reporte y
// lleva la cuenta acumulada desde que arranco el programa.
// ============================================================

class MonitorRecursos {
private:
    unsigned long long iteracionesAcumuladasDesdeElInicio;
    std::size_t memoriaAcumuladaDesdeElInicio;

public:
    MonitorRecursos();

    // nombresDeModulos, iteracionesPorModulo y memoriaPorModulo son tres
    // arreglos "paralelos": la posicion 0 de los tres describe al mismo
    // modulo, la posicion 1 al siguiente, y asi. cantidadDeModulosReportados
    // dice cuantas posiciones tienen esos arreglos.
    void mostrarReporte(const char* const nombresDeModulos[],
                         const unsigned long long iteracionesPorModulo[],
                         const std::size_t memoriaPorModulo[],
                         int cantidadDeModulosReportados);

    void limpiarPantalla();

    unsigned long long getIteracionesAcumuladasDesdeElInicio() const;
    std::size_t getMemoriaAcumuladaDesdeElInicio() const;
};

#endif // MONITOR_RECURSOS_H
