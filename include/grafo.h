#ifndef GRAFO_H
#define GRAFO_H

#include <limits.h>
#include "hash.h"

#define MAX_PONTOS_REDE 100
#define REDE_SEM_ARESTA (-1)
// Limita os pesos para que ate MAX_PONTOS_REDE arestas caibam em int.
// INT_MAX fica reservado como infinito durante os algoritmos.
#define REDE_CUSTO_MAXIMO ((INT_MAX - 1) / MAX_PONTOS_REDE)

#define REDE_ERRO (-1)
#define REDE_SEM_SOLUCAO 0
#define REDE_OK 1

typedef struct
{
    char regiao[TAM_REGIAO];
    double lat, lng; // media das coordenadas das ocorrencias da regiao
} PontoRede;

typedef struct
{
    int quantidade;
    PontoRede pontos[MAX_PONTOS_REDE];
    // Grafo nao direcionado: custos[u][v] == custos[v][u].
    // Valores em centesimos de km; zero e um peso valido.
    // Diagonal e conexoes ausentes usam REDE_SEM_ARESTA.
    int custos[MAX_PONTOS_REDE][MAX_PONTOS_REDE];
} Rede;

typedef struct
{
    int origem, destino; // indices internos, de 0 a quantidade - 1
    int custo; // centesimos de km
} ArestaRede;

// Rede vazia; toda a matriz recebe REDE_SEM_ARESTA.
void rede_limpar(Rede *rede);

// Adiciona regiao unica e coordenadas finitas em [-90, 90] e [-180, 180].
// Retorna o indice interno ou -1 em erro (nome vazio/longo, duplicata ou limite).
int rede_adicionar(Rede *rede, const char *regiao, double lat, double lng);

// Liga nos dois sentidos; retorna 1 se incluiu, 0 se invalida ou repetida.
// Rejeita indices invalidos, lacos e custo fora de [0, REDE_CUSTO_MAXIMO].
int rede_ligar(Rede *rede, int origem, int destino, int custo);

// Le a Hash, agrupa regioes e calcula coordenadas medias, sem alterar a base.
// Ordena regioes por strcmp; liga cada indice aos sucessores +1 e +2,
// circularmente, sem lacos nem duplicatas. Peso: distancia_km * 100,
// arredondada para o inteiro mais proximo (metades para cima).
// Retorna numero de vertices; -1 se faltar espaco ou houver dados invalidos.
// Em erro, deixa a rede vazia. Hash vazia gera rede vazia (retorno 0).
// Regiao vazia e nome invalido sao erros; nao descartar registros em silencio.
int rede_inicializar(Rede *rede, const TabelaHash *tabela);

#endif
