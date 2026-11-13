#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

int eliminar(const char *ruta); // declaracion de funcion para eliminar en el comando rm

int main(void)
{
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
            // comando cp
        }
        else if (strcmp(cmd, "mv") == 0)
        {
            // comando mv
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
                int rm = eliminar(arg1); // va a devolver un entero
                if (rm != 0)
                { // si no devuelve 0 es porque hubo un error
                    printf("No se pudo eliminar %s\n", arg1);
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
    }
    return 0;
}

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
        if (eliminar_nodir == -1) // si hubo un error al eliminar
        {
            int error_archivo = errno;
            return error_archivo;
        }
        return 0;
    }
    else
    {
        DIR *r = opendir(ruta); // abre el directorio
        if (r == NULL) // si hay error al abrir
        {
            int error_carpeta = errno;
            return error_carpeta;
        }
        else
        {
            struct dirent *e; // struct dirent es una entrada de directorio, seria cada uno de los archivos o carpetas dentro de r, e es un puntero
            while ((e = readdir(r)) != NULL) // bucle recursivo
            {
                if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) // si intento eliminar con . o ..
                {
                    continue; // como . y .. aparecen en todas las carpetas, esto hace que termine la recursividad
                }
                char hijo[512]; 
                snprintf(hijo, sizeof(hijo), "%s/%s", ruta, e->d_name);
                int resultado = eliminar(hijo); // recursividad
                if (resultado != 0) // si no da 0 es error, salgo de la carpeta
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
            } else {
                return final;
            }
        }
    }
}