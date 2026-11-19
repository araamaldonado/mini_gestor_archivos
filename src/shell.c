#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include "../include/validaciones.h"
#include "../include/copiar.h"
#include "../include/mover.h"
#include "../include/auditoria.h"
#include "../include/eliminar.h"
#include "../include/listar.h"

int main(void)
{
    auditoria_iniciar(); // inicio el audio_log
    char linea[256];     // cantidad max de caracteres de la linea
    while (1)
    {
        printf("minishell> "); // lo que ve el usuario
        if (fgets(linea, sizeof linea, stdin) == NULL)
        {
            break;
        } // lee lo que ingresa el usuario por teclado en stdin, lo guarda en linea y confirma que no se pase de 256, si se acaba la entrada (Ctrl+D), salgo del bucle
        // cuando no hay \n, nos pasamos de los 255, hay que registrar error de linea larga

        if (strchr(linea, '\n') == NULL)
        { // strchr busca la primera ocurrencia, si da NULL es porque no esta \n
            int c;
            while ((c = getchar()) != '\n' && c != EOF) // borra el resto
            {
            } 
            printf("Error: comando supera los 255 caracteres.\n");
            registrar("Linea demasiado larga", EINVAL); // registra el error 
            continue; // vuelve
                }

        linea[strcspn(linea, "\n")] = '\0'; // elimina el \n automatico despues de la linea, recorre la linea hasta llegar a \n y lo reemplaza por \0, eliminandolo

        char linea_completa[256];
        strcpy(linea_completa, linea); // creo una copia de linea para utilizar en registrar con mv y rm

        char *cmd = strtok(linea, " "); // separa la linea a partir de espacios, la primera palabra es el comando
        if (cmd == NULL)
            continue;                   // si no ingreso nada no se rompe, vuelve a preguntar
        char *arg1 = strtok(NULL, " "); // segunda palabra es null o argumento
        char *arg2 = strtok(NULL, " "); // tercera palabra es null o argumento
        char *arg3 = strtok(NULL, " "); // creado para asegurar que no usen argumentos de mas

        int codigo = 0;                  // inicializo la variable que voy a usar como codigo en la funcion registrar
        const char *operacion_log = cmd; // para utilizar en registrar con el resto

        if (strcmp(cmd, "ls") == 0) // compara la palabra que guardamos en cmd con ls, si es ls lista los archivos
        {
            operacion_log = linea_completa;

            if (arg2 != NULL)
            {
                printf("Demasiados argumentos\n");
                codigo = EINVAL;
            } else {
            codigo = listar(arg1); // llamado a la funcion copiar
            if (codigo != 0)
            { // si no da 0 es porque hubo un error
                printf("No se pudo listar porque %s\n", strerror(codigo));
            }
            }
        }
        else if (strcmp(cmd, "cp") == 0)
        {
            operacion_log = linea_completa;

            if (arg1 == NULL || arg2 == NULL)
            {
                printf("Debe ingresar una ruta de origen y destino.\n"); // no puede ser null
                codigo = EINVAL;
            }
            else if (arg3 != NULL)
            {
                printf("Demasiados argumentos\n");
                codigo = EINVAL;
            }
            else
            {
                codigo = copiar(arg1, arg2); // llamado a la funcion copiar
                if (codigo != 0)
                { // si no da 0 es porque hubo un error
                    printf("No se pudo copiar porque %s\n", strerror(codigo));
                }
                else
                {
                    printf("%s copiado con éxito en %s\n", arg1, arg2);
                }
            }
        }
        else if (strcmp(cmd, "mv") == 0)
        {
            operacion_log = linea_completa; // guardo info necesaria
            if (arg1 == NULL || arg2 == NULL)
            {
                printf("Debe ingresar una ruta de origen y destino.\n"); // no puede ser null
                codigo = EINVAL;                                         // ruta invalida
            }
            else if (arg3 != NULL)
            {
                printf("Demasiados argumentos\n");
                codigo = EINVAL;
            }
            else if (validar_distintos(arg1, "audit.log") != 0 || validar_distintos(arg2, "audit.log") != 0)
            {
                printf("No se puede mover el archivo auditoria\n");
                codigo = EPERM;
            }
            else
            {
                codigo = mover(arg1, arg2);     // llamado a la funcion mover
                if (codigo != 0)
                { // si no da 0 es porque hubo un error
                    printf("No se pudo mover porque %s\n", strerror(codigo));
                }
                else
                {
                    printf("Se movio %s a %s con éxito\n", arg1, arg2);
                }
            }
        }
        else if (strcmp(cmd, "rm") == 0)
        {
            operacion_log = linea_completa;
            if (arg1 == NULL)
            {
                printf("Debe ingresar una ruta.\n"); // no puede ser null
                codigo = EINVAL;
            }
            else if (arg2 != NULL)
            {
                printf("Demasiados argumentos\n");
                codigo = EINVAL;
            }
            else if (validar_distintos(arg1, "audit.log") != 0)
            {
                printf("No se puede eliminar el archivo auditoria\n");
                codigo = EPERM;
            }

            else
            {
                char *carpeta_actual = getcwd(NULL, 0); // asigna dinamicamente un buffer con el espacio para guardar la direccion actual,
                // NULL es el buffer,
                // pasar 0 como tamaño le indica al sistema que pase el numero exacto de bytes y el /0
                // retorna puntero
                char *ruta_real = realpath(arg1, NULL); // resuelve referencias como ., .. y barras extras
                size_t tam = (ruta_real != NULL) ? strlen(ruta_real) : 0;         // length ruta

                if (carpeta_actual == NULL)
                {
                    perror("getcwd");
                    codigo = errno;
                }
                else if (ruta_real != NULL && (strcmp(ruta_real, "/") == 0 || strcmp(carpeta_actual, ruta_real) == 0 || (carpeta_actual[tam] == '/' && strncmp(carpeta_actual, ruta_real, tam) == 0)))
                {
                    printf("No se permite eliminar %s\n", arg1);
                    codigo = EPERM; // operacion no permitida
                }
                else
                {
                    codigo = eliminar(arg1); // va a devolver un entero
                    if (codigo != 0)
                    { // si no devuelve 0 es porque hubo un error
                        printf("No se pudo eliminar %s porque %s\n", arg1, strerror(codigo));
                    }
                    else
                    {
                        printf("Eliminado %s con exito\n", arg1);
                    }
                }
                free(carpeta_actual); // estaban en el heap y el free() libera la memoria
                free(ruta_real);
            }
        }
        else if (strcmp(cmd, "help") == 0)
        {
            printf("Comandos disponibles:\n");
            printf("  ls [ruta]           lista el contenido de una carpeta\n");
            printf("  cp origen destino   copia un archivo\n");
            printf("  mv origen destino   mueve o renombra un archivo o carpeta\n");
            printf("  rm ruta             elimina un archivo o carpeta (recursivo)\n");
            printf("  help                muestra esta ayuda\n");
            printf("  exit                sale de la shell\n");
        }
        else if (strcmp(cmd, "exit") == 0) // exit para salir
        {
            registrar(cmd, 0);
            break;
        }
        else
        {
            printf("Comando desconocido, escriba 'help' para ver la lista de comandos.\n");
            codigo = ENOSYS; // funcion no implementada
        }
        registrar(operacion_log, codigo);
    }
    auditoria_cerrar();
    return 0;
}
