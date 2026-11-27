#ifndef PRIM_H
#define PRIM_H

#include "grafo.h"

typedef struct
{
    int quantidade; // V - 1 arestas quando a rede e conectada e nao vazia
    ArestaRede arestas[MAX_PONTOS_REDE - 1];
    int custo_total; // centesimos de km
} ArvoreRede;

// Parte do indice 0. Retornos: REDE_OK, REDE_SEM_SOLUCAO (vazia/desconexa)
// ou REDE_ERRO (argumentos invalidos). Resultado vazio em falha;
// rede com um vertice tem arvore valida com zero arestas e custo zero.
// Recebe rede construida pelas funcoes acima; usa o mesmo desempate de Dijkstra.
int rede_prim(const Rede *rede, ArvoreRede *arvore);

#endif
