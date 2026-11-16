#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "copiar.h"
#include "validaciones.h"

int copiar(const char *origen, const char *destino)
{
    int r = validar_para_cp(origen, destino);
    if (r != 0) return r;

    int src = open(origen, O_RDONLY);
    if (src == -1) return errno;

    struct stat st;

    int status = fstat(src, &st);

    if (status == -1) {
        int err = errno;
        close(src);
        return err;
    } 

    int dst = open(destino, O_WRONLY | O_CREAT | O_EXCL, st.st_mode & 0777);

    if (dst == -1) {
        int err = errno;
        close(src);
        return err;
    }

    // bucle de lectura y escritura

    char buf[4096];
    ssize_t n = 0;
    int err = 0;

    while (err == 0 && (n = read(src, buf, sizeof buf)) > 0){
        ssize_t escritos = 0;

        while (escritos < n){
            ssize_t w = write(dst, buf + escritos, n - escritos);
            
            if (w == -1){
                err = errno;
                break;
            } else {
                escritos += w;
            }
        }
    }

    if (err == 0 && n == -1){
        err = errno;
    }

    close(src);


    if (close(dst) ==  -1 && err == 0){
        err = errno;   
    }

    if (err != 0){
        unlink(destino);
    }
    return err;
}