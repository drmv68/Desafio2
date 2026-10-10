#ifndef ESTRUCTURAS_BASE_H
#define ESTRUCTURAS_BASE_H

// ============================================================
// core/estructuras_base.h
// Modulo 0.1 del analisis: "Statics, estructuras y archivos".
// Aqui viven los objetos ligeros y esqueletos que se reutilizan
// entre modulos dinamicos (busquedas eficientes por nombre/ID,
// representacion basica de un mensaje para listas por fecha, etc).
//
// FASE MACRO: solo el cascaron de las clases. Sin atributos ni
// logica interna todavia, eso se arma cuando aterricemos cada
// modulo dinamico.
// ============================================================

class UsuarioLigero {
    // Atributos macro: ID, Nombre, ruta_fotos
    // Pensado para busqueda rapida sin cargar el perfil completo.
};

class EstructuraMensaje {
    // Atributos macro: id_remitente, fecha, ruta_archivo
    // Futura sobrecarga de operadores para comparar fechas ira aqui.
};

#endif // ESTRUCTURAS_BASE_H
