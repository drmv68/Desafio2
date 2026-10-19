# Copiar ejemplos solo cuando no existe el archivo de destino. Recompilar no
# debe reemplazar solicitudes, lecturas o likes ya guardados por el programa.
file(GLOB_RECURSE datos RELATIVE "${ORIGEN}" "${ORIGEN}/*")
foreach(archivo IN LISTS datos)
    if(NOT EXISTS "${DESTINO}/${archivo}")
        get_filename_component(carpeta "${DESTINO}/${archivo}" DIRECTORY)
        file(MAKE_DIRECTORY "${carpeta}")
        configure_file("${ORIGEN}/${archivo}" "${DESTINO}/${archivo}" COPYONLY)
    endif()
endforeach()
