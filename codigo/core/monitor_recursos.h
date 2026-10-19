#ifndef MONITOR_RECURSOS_H
#define MONITOR_RECURSOS_H
#include <cstddef>

// Registra datos de una operacion que solo existen mientras su ambito esta vivo.
class MemoriaTemporal {
    std::size_t bytes;
    static std::size_t vivos;

  public:
    explicit MemoriaTemporal(std::size_t cantidad);
    ~MemoriaTemporal();
    MemoriaTemporal(const MemoriaTemporal &) = delete;
    MemoriaTemporal &operator=(const MemoriaTemporal &) = delete;
    static std::size_t getBytesVivos();
};
class MonitorRecursos {
    unsigned long long iteracionesAcumuladasDesdeElInicio;
    std::size_t memoriaActual;

  public:
    MonitorRecursos();
    static void registrarIteracion(int modulo);
    static unsigned long long getIteracionesModulo(int modulo);
    static void reiniciarModulo(int modulo);
    static void pausarConteo(bool pausa);
    static void registrarComponente(unsigned componente);
    static unsigned getComponentes();
    static void reiniciarComponentes();
    void mostrarReporte(const char *const nombres[], const unsigned long long iteraciones[],
                        const std::size_t memoria[], int cantidad);
    void limpiarPantalla();
    unsigned long long getIteracionesAcumuladasDesdeElInicio() const;
    std::size_t
    getMemoriaAcumuladaDesdeElInicio() const; // Compatibilidad: ahora devuelve memoria viva.
};
#endif
