#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include "copiar.h"
#include "mover.h"
#include "auditoria.h"
#include "eliminar.h"


int main(void)
{
    auditoria_iniciar(); // inicio el audio_log
    char linea[256]; // cantidad max de caracteres de la linea
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
            const char *ruta; // declaro variable ruta
            if (arg1 == NULL) // si no puse ninguna, la ruta es la carpeta actual
            {
                ruta = ".";
            }
            else // si puse el nombre de la ruta, uso esa. si pongo ".." lee la carpeta de arriba
            {
                ruta = arg1;
            }

            DIR *d = opendir(ruta); // pide acceso a la carpeta y se guarda la direccion en d
            if (d == NULL)
            {
                int error_carpeta = errno;
                printf("Ls no se pudo abrir: %s\n", strerror(error_carpeta));
            }
            else
            {
                struct dirent *e; // struct dirent es una entrada de directorio, seria cada uno de los archivos o carpetas dentro de d, e es un puntero
                while ((e = readdir(d)) != NULL)
                {                              // recorre la carpeta hasta que no exista el siguiente (readdir(d))
                    printf("%s\n", e->d_name); // print del nombre del archivo o carpeta actual, va a la direccion e (ya que es puntero) y toma la variable d_name
                }
                closedir(d); // libera el acceso a la carpeta
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
            }
        }
        else if (strcmp(cmd, "mv") == 0)
        {
            if (arg1 == NULL || arg2 == NULL)
            {
                printf("Debe ingresar una ruta de origen y destino.\n"); // no puede ser null
            }
            else
            {
                int resultado = mover(arg1, arg2); // llamado a la funcion mover
                if (resultado != 0)
                { // si no da 0 es porque hubo un error
                    int error_mover = errno;
                    printf("No se pudo mover porque %s\n", strerror(error_mover));
                }
            }
        }
        else if (strcmp(cmd, "rm") == 0)
        {
            if (arg1 == NULL)
            {
                printf("Debe ingresar una ruta.\n"); // no puede ser null
            }
            else if (strcmp(arg1, ".") == 0 || strcmp(arg1, "..") == 0 || strcmp(arg1, "/") == 0) // tenemos que ingresar la ruta a eliminar
            {
                printf("No se permite eliminar %s\n", arg1);
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
        }
        else if (strcmp(cmd, "exit") == 0) // exit para salir
        {
            break;
        }
        else
        {
            printf("Comando desconocido\n");
        }
        registrar(cmd, codigo);
    }
    auditoria_cerrar();
    return 0;
}