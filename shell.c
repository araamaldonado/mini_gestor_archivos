#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include "validaciones.h"
#include "copiar.h"
#include "mover.h"
#include "auditoria.h"
#include "eliminar.h"
#include "listar.h"

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
        linea[strcspn(linea, "\n")] = '\0'; // elimina el \n automatico despues de la linea, recorre la linea hasta llegar a \n y lo reemplaza por \0, eliminandolo

        char *cmd = strtok(linea, " "); // separa la linea a partir de espacios, la primera palabra es el comando
        if (cmd == NULL)
            continue;                   // si no ingreso nada no se rompe, vuelve a preguntar
        char *arg1 = strtok(NULL, " "); // segunda palabra es null o argumento
        char *arg2 = strtok(NULL, " "); // tercera palabra es null o argumento

        int codigo = 0; // inicializo la variable que voy a usar como codigo en la funcion registrar

        if (strcmp(cmd, "ls") == 0) // compara la palabra que guardamos en cmd con ls, si es ls lista los archivos
        {

            codigo = listar(arg1); // llamado a la funcion copiar
            if (codigo != 0)
            { // si no da 0 es porque hubo un error
                printf("No se pudo listar porque %s\n", strerror(codigo));
            }
        }
        else if (strcmp(cmd, "cp") == 0)
        {
            if (arg1 == NULL || arg2 == NULL)
            {
                printf("Debe ingresar una ruta de origen y destino.\n"); // no puede ser null
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
            if (arg1 == NULL || arg2 == NULL)
            {
                printf("Debe ingresar una ruta de origen y destino.\n"); // no puede ser null
                codigo = EINVAL;                                         // ruta invalida
            }
            else
            {
                codigo = mover(arg1, arg2); // llamado a la funcion mover
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
            if (arg1 == NULL)
            {
                printf("Debe ingresar una ruta.\n"); // no puede ser null
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

                if (carpeta_actual == NULL)
                {
                    perror("getcwd");
                    codigo = errno;
                }
                else if (ruta_real != NULL && strcmp(ruta_real, carpeta_actual) == 0)
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
        else if (strcmp(cmd, "exit") == 0) // exit para salir
        {
            break;
        }
        else
        {
            printf("Comando desconocido\n");
            codigo = ENOSYS; // funcion no implementada
        }
        registrar(cmd, codigo);
    }
    auditoria_cerrar();
    return 0;
}