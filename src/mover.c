#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "mover.h"
#include "copiar.h"
#include "validaciones.h"

int mover(const char *origen, const char *destino)
{
    int r = validar_para_mv(origen, destino);
    if (r != 0) {
        return r;
    }

    if (rename(origen, destino) == 0){
        return 0;
    }

    // err no es EXDEV
    int err = errno;
    if (err != EXDEV){
        return err;
    }

    // si es EXDEV
    r = copiar(origen, destino);
    if (r != 0){
        return r;
    }

    if (unlink(origen) == -1){
        err = errno;
        return err;
    }

    return 0;
}