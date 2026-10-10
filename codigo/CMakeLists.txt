cmake_minimum_required(VERSION 3.16)

project(desafioDOS_UdeABook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(desafioDOS_UdeABook main.cpp
    core/estructuras_base.h
    core/estructuras_base.cpp
    core/gestor_archivos.h
    core/gestor_archivos.cpp
    core/monitor_recursos.h
    core/monitor_recursos.cpp
    modulos_dinamicos/autenticacion.h
    modulos_dinamicos/autenticacion.cpp
    modulos_dinamicos/admin_amistades.h
    modulos_dinamicos/admin_amistades.cpp
    modulos_dinamicos/visor_perfiles.h
    modulos_dinamicos/visor_perfiles.cpp
    modulos_dinamicos/gestor_mensajes.h
    modulos_dinamicos/gestor_mensajes.cpp
)

include(GNUInstallDirs)
install(TARGETS desafioDOS_UdeABook
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
