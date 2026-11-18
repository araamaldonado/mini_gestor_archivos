#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

int eliminar(const char *ruta)
{
    struct stat st;               // estructura de datos vacia que vamos a rellenar
    int datos = lstat(ruta, &st); // carga los datos de ruta en st, asi podemos preguntar luego el tipo (si es un archivo o carpeta)

    if (datos == -1) // si da un error en el lstat
    {
        int error_lstat = errno;
        return error_lstat;
    }

    if (S_ISDIR(st.st_mode) == 0) // si no es un directorio, es decir un archivo u otro que no sea carpeta
    {
        int eliminar_nodir = unlink(ruta); // caso base
        if (eliminar_nodir == -1)          // si hubo un error al eliminar
        {
            int error_archivo = errno;
            return error_archivo;
        }
        return 0;
    }
    else
    {
        DIR *r = opendir(ruta); // abre el directorio
        if (r == NULL)          // si hay error al abrir
        {
            int error_carpeta = errno;
            return error_carpeta;
        }
        else
        {
            struct dirent *e;                // struct dirent es una entrada de directorio, seria cada uno de los archivos o carpetas dentro de r, e es un puntero
            while ((e = readdir(r)) != NULL) // bucle recursivo
            {
                if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) // si intento eliminar con . o ..
                {
                    continue; // como . y .. aparecen en todas las carpetas, esto hace que termine la recursividad
                }
                char hijo[512];
                snprintf(hijo, sizeof(hijo), "%s/%s", ruta, e->d_name);
                int resultado = eliminar(hijo); // recursividad
                if (resultado != 0)             // si no da 0 es error, salgo de la carpeta
                {
                    closedir(r);
                    return resultado;
                }
            }
            closedir(r); // salgo de la carpeta
            int final = rmdir(ruta);
            if (final != 0)
            {
                int error_final = errno;
                return error_final;
            }
            else
            {
                return final;
            }
        }
    }
}