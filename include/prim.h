#ifndef PRIM_H
#define PRIM_H

#include "grafo.h"

typedef struct
{
    int quantidade; // V - 1 arestas quando a rede e conectada e nao vazia
    ArestaRede arestas[MAX_PONTOS_REDE - 1];
    int custo_total; // centesimos de km
} ArvoreRede;

// Parte do indice 0. Retorna REDE_OK, REDE_SEM_SOLUCAO ou REDE_ERRO.
// Rede vazia/desconexa: saida vazia. Um vertice: arvore valida de custo zero.
// Usa grafo valido e desempata pelo menor indice.
int rede_prim(const Rede *rede, ArvoreRede *arvore);

#endif
