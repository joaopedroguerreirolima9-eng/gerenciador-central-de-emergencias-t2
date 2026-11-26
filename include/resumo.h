#ifndef RESUMO_H
#define RESUMO_H

#include "otimizacao.h"

typedef struct
{
    char regiao[TAM_REGIAO]; // vazio e exibido como "(sem regiao)"
    int quantidade, minutos, beneficio;
} ResumoRegiao;

typedef struct
{
    ResumoRegiao *regioes; // vetor alocado por resumo_calcular
    int quantidade_regioes;
    int selecionadas, minutos, beneficio;
    int maior_quantidade;
} ResumoPlano;

// Usa os mesmos itens e a mesma mascara da mochila, sem executar otimizacao.
// Os candidatos devem ter IDs unicos, como os obtidos pelo modulo 7.
// Retorna 1 em sucesso (inclusive selecao vazia), 0 em erro, com saida vazia.
// IDs selecionados devem existir na Hash. Regioes sao ordenadas por nome.
// A saida deve ser nova ou ter sido liberada antes de uma nova chamada.
// Em sucesso, chamar resumo_liberar quando terminar de usar o resultado.
int resumo_calcular(TabelaHash *tabela, const ItemPlano *itens,
                    const int *escolhido, int n, ResumoPlano *resumo);

void resumo_mostrar(const ResumoPlano *resumo);
void resumo_liberar(ResumoPlano *resumo);

#endif
