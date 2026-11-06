#include <stdio.h>
#include <string.h>

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
            if (arg1 == NULL)
            {
                printf("comando: %s \n", cmd);
            }
            else
            {
                printf("comando: %s, argumento: %s \n", cmd, arg1); // practica para ver si funciona
            }
        }
        else if (strcmp(cmd, "cp") == 0)
        {
            if (arg1 == NULL || arg2 == NULL)
            {
                printf("Faltan argumentos \n");
            }

            else
            {
                printf("comando: %s, argumentos: %s y %s \n", cmd, arg1, arg2); // practica para ver si funciona
            }
        }
        else if (strcmp(cmd, "mv") == 0)
        {
            if (arg1 == NULL)
            {
                printf("comando: %s \n", cmd);
            }
            else
            {
                printf("comando: %s, argumento: %s \n", cmd, arg1); // practica para ver si funciona
            }
        }
        else if (strcmp(cmd, "rm") == 0)
        {
            if (arg1 == NULL)
            {
                printf("comando: %s \n", cmd);
            }
            else
            {
                printf("comando: %s, argumento: %s \n", cmd, arg1); // practica para ver si funciona
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