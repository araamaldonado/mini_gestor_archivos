# Mini gestor de archivos con auditoría

TP integrador de Sistemas Operativos (UNAHUR, 2.º cuatrimestre 2026), opción 4: Sistema de Archivos.

Mini-shell en C para Linux/WSL que permite listar, copiar, mover y eliminar archivos y carpetas. Cada operación queda registrada en un log de auditoría (`audit.log`). La copia usa llamadas al sistema de bajo nivel (`open`, `read`, `write`, `close`), sin funciones de copia de alto nivel de la biblioteca estándar.

## Autoras

- Maldonado, Araceli: validaciones, copia, movimiento y auditoría.
- Movia, Martina: shell, eliminación y listado.

## Estructura

```
.
├── Makefile
├── include/          # encabezados (.h)
│   ├── auditoria.h
│   ├── copiar.h
│   ├── eliminar.h
│   ├── listar.h
│   ├── mover.h
│   └── validaciones.h
└── src/              # implementación (.c)
    ├── auditoria.c
    ├── copiar.c
    ├── eliminar.c
    ├── listar.c
    ├── mover.c
    ├── shell.c
    └── validaciones.c
```

## Compilar y ejecutar

Requiere `gcc` y `make` en Linux o WSL.

```bash
make            # genera el ejecutable shell_test
./shell_test    # inicia la shell
make clean      # borra el ejecutable
```

Compilación manual equivalente:

```bash
gcc -Wall -Wextra -Iinclude -o shell_test src/*.c
```

## Comandos

| Comando | Uso | Descripción |
|---|---|---|
| `ls` | `ls [directorio]` | Lista el contenido del directorio indicado (por defecto, el actual) |
| `cp` | `cp origen destino` | Copia un archivo regular; no sobrescribe destinos existentes |
| `mv` | `mv origen destino` | Mueve o renombra; entre sistemas de archivos distintos copia y luego borra el origen |
| `rm` | `rm ruta` | Elimina un archivo, enlace simbólico o directorio (recursivo) |
| `help` | `help` | Muestra los comandos disponibles |
| `exit` | `exit` | Cierra la shell |

## Validaciones

Antes de operar, se comprueba que:

- el origen exista y el destino no exista (no se sobrescribe);
- origen y destino no sean el mismo archivo (se compara `st_dev` y `st_ino`, no el texto de la ruta);
- `cp` reciba solo archivos regulares;
- no se pueda borrar la carpeta actual ni mover o eliminar `audit.log`;
- los permisos permitan la operación.

Si faltan argumentos o el comando supera el largo máximo, la shell informa el error por pantalla.

## Auditoría

Cada comando se agrega a `audit.log` con fecha y hora, PID del proceso, la operación (con sus argumentos) y el resultado: `OK` o `FAIL <errno> (<mensaje>)`.

```
2026-11-18 21:04:12 | PID 4821 | cp chico.txt copia.txt | OK
2026-11-18 21:04:30 | PID 4821 | rm noexiste | FAIL 2 (No such file or directory)
```

El archivo se abre una sola vez con `O_APPEND`, así que cada registro se escribe al final de forma atómica. La hora se registra en UTC si el sistema (por ejemplo, WSL) tiene el reloj en UTC.

## Diseño

- **Contrato de retorno:** todas las funciones de los módulos devuelven `0` si salió bien o un código `errno` si falló.
- **Separación de responsabilidades:** los módulos no imprimen ni escriben en el log. Solo el bucle principal de `shell.c` muestra mensajes y llama a `registrar()`.

## Limitaciones

- `cp` solo copia archivos regulares, por lo que mover un directorio entre sistemas de archivos distintos no está soportado.
- La eliminación recursiva no es transaccional: si falla a mitad de camino puede quedar un borrado parcial.

## Posibles mejoras

- Copia recursiva de directorios con manejo de enlaces simbólicos.
- Verificación de integridad por hash al copiar.
- Comando de restauración basado en el log.