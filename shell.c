#include <stdio.h>
#include <string.h>

int main()
{
    while (1)
    {
        char linea[256];                   // cantidad max de caracteres de la linea
        printf("minishell>");              // lo que ve el usuario
        fgets(linea, sizeof linea, stdin); // lee lo que ingresa el usuario por teclado en stdin, lo guarda en linea y confirma que no se pase de 256
        linea[strcspn(linea, "\n")] = '\0'; // elimina el \n automatico despues de la linea, recorre la linea hasta llegar a \n y lo reemplaza por \0, eliminandolo

        char *cmd = strtok(linea, " "); // separa la linea a partir de espacios, la primera palabra es el comando
        char *arg1 = strtok(NULL, " "); // segunda palabra es null o argumento
        char *arg2 = strtok(NULL, " "); // tercera palabra es null o argumento

        if (strcmp(cmd, "ls") == 0) // compara la palabra que guardamos en cmd con ls, si es ls lista los archivos
        {
        }
    }
}