#ifndef OTIMIZACAO_H
#define OTIMIZACAO_H
#include "hash.h"
#include "indices.h"
#define MAX_MINUTOS 1440

typedef struct
{
    int id, custo, beneficio;
} ItemPlano;

ItemPlano plano_item(const Ocorrencia *o);
// Retorna -1 em erro ou o beneficio maximo; escolhido[i] vale 0 ou 1.
int plano_mochila(const ItemPlano *itens, int n, int capacidade, int *escolhido);
void otimizacao_menu(TabelaHash *tabela, Indices *indices);
void plano_mostrar_dados(const Ocorrencia *o);
void plano_consultar_dados(TabelaHash *tabela);
#endif
