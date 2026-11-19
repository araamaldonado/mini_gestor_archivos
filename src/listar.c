#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>

int listar(const char *ruta)
{                     
    if (ruta == NULL) // si no puse ninguna, la ruta es la carpeta actual
    {
        ruta = ".";
    }
    DIR *d = opendir(ruta); // pide acceso a la carpeta y se guarda la direccion en d
    if (d == NULL)
    {
        int error_carpeta = errno;
        return error_carpeta;
    }
    else
    {
        struct dirent *e; // struct dirent es una entrada de directorio, seria cada uno de los archivos o carpetas dentro de d, e es un puntero
        while ((e = readdir(d)) != NULL)
        {                              // recorre la carpeta hasta que no exista el siguiente (readdir(d))
            printf("%s\n", e->d_name); // print del nombre del archivo o carpeta actual, va a la direccion e (ya que es puntero) y toma la variable d_name
        }
        closedir(d); // libera el acceso a la carpeta
        return 0;
    }
}