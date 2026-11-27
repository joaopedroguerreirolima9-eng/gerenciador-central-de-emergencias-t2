#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "grafo.h"

typedef struct
{
    int quantidade;
    int vertices[MAX_PONTOS_REDE]; // caminho em ordem, da origem ao destino
    int custo_total; // centesimos de km
} CaminhoRede;

// Retornos: REDE_OK, REDE_SEM_SOLUCAO (sem caminho) ou REDE_ERRO
// (argumentos invalidos). Origem igual ao destino: 1 vertice, custo zero.
// Recebe rede construida pelas funcoes acima. Resultado vazio em falha.
// Empate: menor indice na escolha do vertice; so atualiza em custo menor.
int rede_dijkstra(const Rede *rede, int origem, int destino, CaminhoRede *caminho);

#endif
