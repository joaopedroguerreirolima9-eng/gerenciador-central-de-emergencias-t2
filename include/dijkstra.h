#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "grafo.h"

typedef struct
{
    int quantidade;
    int vertices[MAX_PONTOS_REDE]; // caminho em ordem, da origem ao destino
    int custo_total; // centesimos de km
} CaminhoRede;

// Retorna REDE_OK, REDE_SEM_SOLUCAO ou REDE_ERRO; saida vazia em falha.
// Usa grafo valido. Origem igual ao destino: um vertice, custo zero.
// Empate: menor indice na escolha do vertice; so atualiza em custo menor.
int rede_dijkstra(const Rede *rede, int origem, int destino, CaminhoRede *caminho);

#endif
