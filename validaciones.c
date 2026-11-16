#include "validaciones.h"
#include <errno.h>
#include <sys/stat.h>
#include <stddef.h>

int validar_existe(const char *ruta){
    struct stat st;
    if (lstat(ruta, &st) == -1){
        int err = errno;
        return err;
    }

    return 0;
}

int validar_destino_libre(const char *destino){
    struct stat st;

    if (lstat(destino, &st) == 0){
        return EEXIST;
    }

    int err = errno;
    
    if (err == ENOENT){
        return 0;
    }
    
    return err;
}

int validar_distintos(const char *origen, const char *destino){
    struct stat a, b;

    if ( lstat(origen, &a) == -1 || lstat(destino, &b) == -1){
        return 0;
    }

    if (a.st_dev == b.st_dev && a.st_ino == b.st_ino){
        return EINVAL;
    }

    return 0;
}

int validar_para_cp(const char *origen, const char *destino){
    struct stat st;
    int r;

    if (origen == NULL || destino == NULL){
        return EINVAL;
    }

    r = validar_existe(origen);

    if (r != 0){
        return r;
    }

    lstat(origen, &st);

    if (S_ISDIR(st.st_mode)){
        return EISDIR;
    }

    if (!S_ISREG(st.st_mode)){
        return EINVAL;
    }

    r = validar_distintos(origen, destino);
    if (r != 0){
        return r;
    }

    return validar_destino_libre(destino);
}

int validar_para_mv(const char *origen, const char *destino){
    int r;

    if (origen == NULL || destino == NULL){
        return EINVAL;
    }

    r = validar_existe(origen);
    if (r != 0){
        return r;
    }

    r = validar_distintos(origen, destino);
    if (r != 0){
        return r;
    }

    return validar_destino_libre(destino);
}