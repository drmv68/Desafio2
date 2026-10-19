#ifndef MENU_H
#define MENU_H
#include "../core/gestor_archivos.h"
#include "../core/monitor_recursos.h"
#include "autenticacion.h"
#include "admin_amistades.h"
#include "visor_perfiles.h"
#include "gestor_mensajes.h"
class Menu {
    GestorArchivos &baseDatos;
    Autenticacion &moduloAcceso;
    AdminAmistades &moduloAmistades;
    VisorPerfiles &moduloPerfiles;
    GestorMensajes &moduloMensajes;
    MonitorRecursos &monitor;
    void cargarUsuariosEnRedDeAmistades();
    void ejecutarOpcion(int opcion);
    void mostrarReporteDeTodos();
    static void terminarOperacion(void *contexto);

  public:
    Menu(GestorArchivos &, Autenticacion &, AdminAmistades &, VisorPerfiles &, GestorMensajes &,
         MonitorRecursos &);
    bool iniciar(const char *rutaIndiceUsuarios);
};
#endif
