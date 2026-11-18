#ifndef VALIDACIONES_H
#define VALIDACIONES_H

int validar_existe(const char *ruta);
int validar_destino_libre(const char *destino);
int validar_distintos(const char *origen, const char *destino);
int validar_para_cp(const char *origen, const char *destino);
int validar_para_mv(const char *origen, const char *destino);

#endif