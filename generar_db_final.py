
import os, random, shutil

random.seed(42)

TOTAL_USUARIOS = 1314
ID_TOM = 2306
ID_ROSA = 2307
ID_GUTI3 = 2308
ID_JUAN = 2309
ID_KEN = 2310
ID_LIZ = 2311
ID_GRECITA = 2312
ID_CESAR = 2313
ID_MILA = 2314

# --- 1. USUARIOS ---
usuarios = [
    (1001, 'pass123', 'daniel mosquera'),
    (1002, 'clave456', 'juan perez'),
    (1003, 'abc789', 'ana maria lopez'),
    (1004, 'carlos2026', 'carlos gomez'),
    (1005, 'sofiapass', 'sofia ramirez'),
    (1006, 'luis123', 'luis fernandez'),
    (1007, 'maria777', 'maria camila'),
    (1008, 'mateo888', 'mateo herrera')
]

for uid in range(1009, 2006):
    usuarios.append((uid, f'pass{uid}', f'usuario {uid}'))

# 300 usuarios aislados (2006 - 2305)
for uid in range(2006, 2306):
    usuarios.append((uid, f'pass{uid}', f'usuario aislado {uid}'))

# Especiales al final
especiales = [
    (ID_TOM, 'passTom', 'Tom'),
    (ID_ROSA, 'passRosa', 'ROSA'),
    (ID_GUTI3, 'passGuti', 'Guti3'),
    (ID_JUAN, 'passJuan', 'Juan'),
    (ID_KEN, 'passKen', 'Ken'),
    (ID_LIZ, 'passLiz', 'LIZ'),
    (ID_GRECITA, 'passGrecita', 'Grecita'),
    (ID_CESAR, 'passCesar', 'César'),
    (ID_MILA, 'passMila', 'MILA')
]
usuarios.extend(especiales)

# --- 2. AMISTADES ---
amistades_set = set()
def add_friendship(u1, u2):
    if u1 == u2: return
    p = (min(u1, u2), max(u1, u2))
    amistades_set.add(p)

# Base 1001-1008
for u1, u2 in [(1001, 1002), (1001, 1003), (1002, 1003), (1002, 1004),
               (1002, 1005), (1003, 1005), (1003, 1006), (1004, 1005),
               (1005, 1007), (1006, 1008)]:
    add_friendship(u1, u2)

# Regulares interconectados
for uid in range(1009, 1100):
    add_friendship(uid, uid + 1)
    add_friendship(uid, 1001 + (uid % 7))

# Amigos de Tom
amigos_tom_grupo = list(range(1010, 1038)) # 28 amigos
for u in amigos_tom_grupo:
    add_friendship(ID_TOM, u)

add_friendship(ID_TOM, ID_GUTI3)
add_friendship(ID_TOM, ID_JUAN)

# Juan comparte al menos 25 amigos en comun con Tom (los 28)
for u in amigos_tom_grupo:
    add_friendship(ID_JUAN, u)

# LIZ, Grecita, César, MILA amigos de ROSA, Guti3, Juan y Ken
for c in [ID_LIZ, ID_GRECITA, ID_CESAR, ID_MILA]:
    for obj in [ID_ROSA, ID_GUTI3, ID_JUAN, ID_KEN]:
        add_friendship(c, obj)

# Grecita amiga de al menos 20 de los amigos de Tom (1010 a 1031 = 22 amigos)
for u in range(1010, 1032):
    add_friendship(ID_GRECITA, u)

# Grecita NO es amiga de Tom
if (min(ID_GRECITA, ID_TOM), max(ID_GRECITA, ID_TOM)) in amistades_set:
    amistades_set.remove((min(ID_GRECITA, ID_TOM), max(ID_GRECITA, ID_TOM)))

# --- 3. SOLICITUDES ---
solicitudes = [
    (1006, 1001),
    (1007, 1001),
    (ID_ROSA, ID_TOM), # ROSA -> Tom
    (ID_KEN, ID_TOM)   # Ken -> Tom
]

# --- 4. MENSAJES ---
mensajes = [
    (1, 1002, 1001, 10, 8, 2026, 'Hola Daniel, bienvenido a Ude@Book!', 1),
    (2, 1002, 1001, 15, 9, 2026, 'Ya revise el informe de Informatica II, quedo excelente.', 0),
    (3, 1003, 1001, 28, 9, 2026, 'Hola! Recuerda subir las fotos de la salida de campo.', 0),
    (4, 1002, 1001, 5, 10, 2026, 'Nos vemos en la sustentacion este viernes.', 0),
    (5, 1001, 1002, 6, 10, 2026, 'Listo Juan, nos encontramos en el bloque 19.', 1)
]

next_msg_id = 6
# Guti3 a Tom (22 mensajes distribuidos de manera uniforme entre 23-dic-2025 y 10-ene-2026)
fechas_guti = [
    (23, 12, 2025, 'Hola Tom, coordinemos el trabajo del proyecto.'),
    (24, 12, 2025, 'Buenas tardes Tom, preparando los modulos.'),
    (25, 12, 2025, 'Feliz Navidad Tom, descansamos hoy.'),
    (25, 12, 2025, 'Por la noche reviso el gestor de mensajes.'),
    (26, 12, 2025, 'Avanzando con la matriz de adyacencia.'),
    (27, 12, 2025, 'Validando memoria dinamica en el heap.'),
    (28, 12, 2025, 'Pruebas de solicitudes listas.'),
    (29, 12, 2025, 'Revisando sobrecarga de operadores en Fecha.'),
    (30, 12, 2025, 'Todo compila perfecto en MinGW.'),
    (31, 12, 2025, 'Feliz fin de ano Tom!'),
    (31, 12, 2025, 'Listo para comenzar el 2026 con toda.'),
    (1, 1, 2026, 'Feliz ano 2026 Tom!'),
    (1, 1, 2026, 'Retomamos el proyecto manana.'),
    (2, 1, 2026, 'Verificando los algoritmos de sugerencias.'),
    (3, 1, 2026, 'Comprobando que no haya fugas de memoria.'),
    (4, 1, 2026, 'Probando la busqueda de mensajes por rango.'),
    (5, 1, 2026, 'Revisando que el carrusel filtre fotos privadas.'),
    (6, 1, 2026, 'Todo en orden con las estadisticas del monitor.'),
    (7, 1, 2026, 'Confirmando las lineas comentadas de limpieza.'),
    (8, 1, 2026, 'Alistando la hoja de formato de evaluacion.'),
    (9, 1, 2026, 'Manana hacemos el ensayo final.'),
    (10, 1, 2026, 'Listo Tom, todos los mensajes de Guti3 quedaron verificados.')
]
for dia, mes, anio, txt in fechas_guti:
    mensajes.append((next_msg_id, ID_GUTI3, ID_TOM, dia, mes, anio, txt, 0))
    next_msg_id += 1

# Juan a Tom (2 mensajes el 31-ene-2026 y 3 el 01-feb-2026)
mensajes.append((next_msg_id, ID_JUAN, ID_TOM, 31, 1, 2026, 'Hola Tom, confirmando que tenemos 28 amigos en comun.', 0))
next_msg_id += 1
mensajes.append((next_msg_id, ID_JUAN, ID_TOM, 31, 1, 2026, 'Revisa que tengamos listo el informe de responsabilidades.', 0))
next_msg_id += 1
mensajes.append((next_msg_id, ID_JUAN, ID_TOM, 1, 2, 2026, 'Buenos dias Tom, hoy ultimamos detalles de la entrega.', 0))
next_msg_id += 1
mensajes.append((next_msg_id, ID_JUAN, ID_TOM, 1, 2, 2026, 'Verifique las iteraciones del monitor y dan exactas.', 0))
next_msg_id += 1
mensajes.append((next_msg_id, ID_JUAN, ID_TOM, 1, 2, 2026, 'Puntuales en el auditorio el miercoles a las 10:05 am.', 1))
next_msg_id += 1

# --- FUNCION PARA ESCRIBIR EN UNA CARPETA BASE ---
def escribir_dataset(base_dir):
    os.makedirs(base_dir, exist_ok=True)
    perfiles_dir = os.path.join(base_dir, 'perfiles')
    os.makedirs(perfiles_dir, exist_ok=True)
    fotos_dir = os.path.join(base_dir, 'fotos')
    os.makedirs(fotos_dir, exist_ok=True)

    # 1. indice.txt
    with open(os.path.join(base_dir, 'indice.txt'), 'w', encoding='utf-8') as f:
        f.write(f'{len(usuarios)}\n')
        for uid, pwd, nom in usuarios:
            f.write(f'{uid};{pwd};{nom}\n')

    # 2. amistades.txt
    with open(os.path.join(base_dir, 'amistades.txt'), 'w', encoding='utf-8') as f:
        f.write(f'{len(amistades_set)}\n')
        for u1, u2 in sorted(amistades_set):
            f.write(f'{u1};{u2}\n')

    # 3. solicitudes.txt
    with open(os.path.join(base_dir, 'solicitudes.txt'), 'w', encoding='utf-8') as f:
        f.write(f'{len(solicitudes)}\n')
        for emisor, receptor in solicitudes:
            f.write(f'{emisor};{receptor}\n')

    # 4. mensajes.txt
    with open(os.path.join(base_dir, 'mensajes.txt'), 'w', encoding='utf-8') as f:
        f.write(f'{len(mensajes)}\n')
        for mid, em, rec, d, m, a, txt, l in mensajes:
            f.write(f'{mid};{em};{rec};{d};{m};{a};{txt};{l}\n')

    # 5. perfiles
    # Perfil especial de ROSA (2307):
    # 'Rosa tiene sólo tres álbumes de fotos: Uno vacío, uno privado, uno llamado SUR1 con dos fotos únicamente.'
    with open(os.path.join(perfiles_dir, f'{ID_ROSA}.txt'), 'w', encoding='utf-8') as f:
        f.write(f'estaticos_y_db/fotos/{ID_ROSA}\n')
        f.write('3\n')
        f.write('SUR1/foto1.png;18;1\n')
        f.write('SUR1/foto2.png;24;1\n')
        f.write('privado/foto_privada.png;10;0\n')

    # Crear estructura física de carpetas y fotos para Rosa
    rosa_fotos = os.path.join(fotos_dir, str(ID_ROSA))
    os.makedirs(os.path.join(rosa_fotos, 'SUR1'), exist_ok=True)
    os.makedirs(os.path.join(rosa_fotos, 'privado'), exist_ok=True)
    os.makedirs(os.path.join(rosa_fotos, 'vacio'), exist_ok=True)
    for p in ['SUR1/foto1.png', 'SUR1/foto2.png', 'privado/foto_privada.png']:
        fp = os.path.join(rosa_fotos, p)
        if not os.path.exists(fp):
            with open(fp, 'w', encoding='utf-8') as f_img:
                f_img.write('DATOS_FOTO_SIMULADA')

    # Perfiles de otros personajes especiales
    otros_especiales = [
        (ID_TOM, [('perfil.png', 35, 1), ('paseo_guatape.png', 12, 1), ('amigos_u.png', 15, 0)]),
        (ID_GUTI3, [('perfil.png', 9, 1), ('futbol.png', 11, 1)]),
        (ID_JUAN, [('perfil.png', 14, 1), ('estudio.png', 8, 1)]),
        (ID_KEN, [('perfil.png', 5, 1), ('musica.png', 10, 1)]),
        (ID_LIZ, [('perfil.png', 22, 1), ('danza.png', 18, 1)]),
        (ID_GRECITA, [('perfil.png', 19, 1), ('viaje_medellin.png', 16, 1)]),
        (ID_CESAR, [('perfil.png', 15, 1), ('ingenieria.png', 12, 1)]),
        (ID_MILA, [('perfil.png', 20, 1), ('fotografia.png', 17, 1)])
    ]
    for uid, fotos_list in otros_especiales:
        with open(os.path.join(perfiles_dir, f'{uid}.txt'), 'w', encoding='utf-8') as f:
            f.write(f'estaticos_y_db/fotos/{uid}\n')
            f.write(f'{len(fotos_list)}\n')
            for fn, lk, pub in fotos_list:
                f.write(f'{fn};{lk};{pub}\n')

# Escribir en codigo/estaticos_y_db
escribir_dataset('codigo/estaticos_y_db')

# Escribir en codigo/build/Desktop_Qt_6_8_7_MinGW_64_bit_Debug/estaticos_y_db
escribir_dataset('codigo/build/Desktop_Qt_6_8_7_MinGW_64_bit_Debug/estaticos_y_db')

print('Dataset escrito exitosamente en ambas ubicaciones!')
