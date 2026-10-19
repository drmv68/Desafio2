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
    modulos_dinamicos/menu.h
    modulos_dinamicos/menu.cpp
)

# Mantener los datos junto al ejecutable sin sobrescribir cambios al recompilar.
add_custom_command(TARGET desafioDOS_UdeABook POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        "-DORIGEN=${CMAKE_SOURCE_DIR}/estaticos_y_db"
        "-DDESTINO=$<TARGET_FILE_DIR:desafioDOS_UdeABook>/estaticos_y_db"
        -P "${CMAKE_SOURCE_DIR}/core/copiar_datos.cmake"
    VERBATIM
)

include(GNUInstallDirs)
install(TARGETS desafioDOS_UdeABook
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Cinco pruebas pequenas de la logica y la integracion completa.
include(CTest)
if(BUILD_TESTING)
    add_executable(pruebas_solarte pruebas/pruebas_solarte.cpp
        modulos_dinamicos/admin_amistades.cpp modulos_dinamicos/gestor_mensajes.cpp
        core/estructuras_base.cpp core/monitor_recursos.cpp core/gestor_archivos.cpp
        modulos_dinamicos/autenticacion.cpp modulos_dinamicos/visor_perfiles.cpp
        modulos_dinamicos/menu.cpp)
    target_compile_definitions(pruebas_solarte PRIVATE
        DATOS_PRUEBA_ORIGEN="${CMAKE_CURRENT_SOURCE_DIR}/estaticos_y_db")
    add_test(NAME solicitudes COMMAND pruebas_solarte 1)
    add_test(NAME sugerencias COMMAND pruebas_solarte 2)
    add_test(NAME validacion_mensajes COMMAND pruebas_solarte 3)
    add_test(NAME fechas_y_lectura COMMAND pruebas_solarte 4)
    add_test(NAME menus COMMAND pruebas_solarte 5)
endif()
option(CONSTRUIR_DEMO_SOLARTE "Ejemplo en memoria con cinco usuarios" OFF)
if(CONSTRUIR_DEMO_SOLARTE)
    add_executable(demo_solarte ejemplos/demo_solarte.cpp
        modulos_dinamicos/admin_amistades.cpp modulos_dinamicos/gestor_mensajes.cpp
        core/estructuras_base.cpp core/monitor_recursos.cpp core/gestor_archivos.cpp
        modulos_dinamicos/autenticacion.cpp modulos_dinamicos/visor_perfiles.cpp
        modulos_dinamicos/menu.cpp)
endif()
