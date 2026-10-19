#ifndef MENU_H
#define MENU_H

#include "../core/gestor_archivos.h"
#include "../core/monitor_recursos.h"
#include "autenticacion.h"
#include "admin_amistades.h"
#include "visor_perfiles.h"
#include "gestor_mensajes.h"

class Menu {
private:
    GestorArchivos& baseDatos;
    Autenticacion& moduloAcceso;
    AdminAmistades& moduloAmistades;
    VisorPerfiles& moduloPerfiles;
    GestorMensajes& moduloMensajes;
    MonitorRecursos& monitor;

    void cargarUsuariosEnRedDeAmistades();
    void ejecutarOpcion(int opcionElegida);
    void mostrarReporteDeTodos();

public:
    Menu(GestorArchivos& baseDatosInicial, Autenticacion& accesoInicial,
         AdminAmistades& amistadesInicial, VisorPerfiles& perfilesInicial,
         GestorMensajes& mensajesInicial, MonitorRecursos& monitorInicial);

    bool iniciar(const char* rutaIndiceUsuarios);
};

#endif
