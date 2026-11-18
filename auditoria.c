#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "auditoria.h"

static int log_fd = -1;

int auditoria_iniciar(void)
{
    log_fd = open("audit.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == -1){
        int err = errno;
        return err;
    }
    return 0;
}

void auditoria_cerrar(void)
{
    if (log_fd != -1){
        close(log_fd);
        log_fd = -1;
    }
}

void registrar(const char *operacion, int codigo)
{
    if (log_fd == -1){
        return;
    }

    time_t ahora = time(NULL);
    char fecha[32];
    strftime(fecha, sizeof fecha, "%Y-%m-%d %H:%M:%S", localtime(&ahora));

    char resultado[128];
    if (codigo == 0){
        snprintf(resultado, sizeof resultado, "OK");
    } else {
        snprintf(resultado, sizeof resultado, "FAIL %d (%s)", codigo, strerror(codigo));
    }

    char linea[512];
    int largo = snprintf(linea, sizeof linea, "%s | PID %d | %s | %s\n", fecha, getpid(), operacion, resultado);// paso 3: línea
    if (largo >= (int)sizeof linea){
        largo = sizeof linea - 1;
    }

    ssize_t escrito = write(log_fd, linea, largo);
    (void)escrito;
}