#ifndef KAHN_H
#define KAHN_H

#include "dependencias.h"

// Retorna quantas atividades foram processadas; menos que V indica ciclo.
int dependencias_kahn(const Dependencias *g, int *ordem, int *grau);

#endif
