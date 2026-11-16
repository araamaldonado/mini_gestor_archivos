#ifndef AUDITORIA_H
#define AUDITORIA_H

int auditoria_iniciar(void);
void auditoria_cerrar(void);
void registrar(const char *operacion, int codigo);

#endif