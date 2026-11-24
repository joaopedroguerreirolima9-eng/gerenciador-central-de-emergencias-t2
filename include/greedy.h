#ifndef GREEDY_H
#define GREEDY_H

#include "hash.h"
#include "indices.h"

// Modulo 5 - Operacao Resgate: monta a ordem de atendimento com um algoritmo guloso.
double distancia_km(double lat1, double lng1, double lat2, double lng2);
int prioridade_categoria(const Ocorrencia *o);
void greedy_menu(TabelaHash *tabela, Indices *indices);

#endif
