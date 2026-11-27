// Agrupamento por regiao em vetor; consulta das ocorrencias pela Tabela Hash.
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resumo.h"

static int resumo_comparar_regiao(const void *a, const void *b)
{
    const ResumoRegiao *x = a;
    const ResumoRegiao *y = b;
    return strcmp(x->regiao, y->regiao);
}

void resumo_liberar(ResumoPlano *resumo)
{
    if (resumo == NULL)
        return;
    free(resumo->regioes);
    memset(resumo, 0, sizeof *resumo);
}

int resumo_calcular(TabelaHash *tabela, const ItemPlano *itens,
                    const int *escolhido, int n, ResumoPlano *resumo)
{
    if (resumo == NULL)
        return 0;
    memset(resumo, 0, sizeof *resumo);
    if (tabela == NULL || n < 0 || (n > 0 && (itens == NULL || escolhido == NULL)))
        return 0;

    int selecionadas = 0;
    for (int i = 0; i < n; i++)
    {
        if (escolhido[i] != 0 && escolhido[i] != 1)
            return 0;
        selecionadas += escolhido[i];
    }
    if (selecionadas == 0)
        return 1;

    ResumoPlano resultado = {0};
    if ((size_t)selecionadas > SIZE_MAX / sizeof *resultado.regioes)
        return 0;
    // No maximo uma regiao por ocorrencia selecionada, sem limite fixo novo.
    resultado.regioes = calloc((size_t)selecionadas, sizeof *resultado.regioes);
    if (resultado.regioes == NULL)
        return 0;

    for (int i = 0; i < n; i++)
    {
        if (!escolhido[i])
            continue;
        if (itens[i].id <= 0 || itens[i].custo <= 0 ||
            itens[i].beneficio < 0 || itens[i].beneficio > 4)
            goto erro;
        Ocorrencia *o = hash_buscar(tabela, itens[i].id);
        if (o == NULL || resultado.minutos > INT_MAX - itens[i].custo ||
            resultado.beneficio > INT_MAX - itens[i].beneficio)
            goto erro;

        int j = 0;
        while (j < resultado.quantidade_regioes &&
               strcmp(resultado.regioes[j].regiao, o->regiao) != 0)
            j++;
        if (j == resultado.quantidade_regioes)
        {
            strcpy(resultado.regioes[j].regiao, o->regiao);
            resultado.quantidade_regioes++;
        }
        ResumoRegiao *regiao = &resultado.regioes[j];
        regiao->quantidade++;
        regiao->minutos += itens[i].custo;
        regiao->beneficio += itens[i].beneficio;
        resultado.selecionadas++;
        resultado.minutos += itens[i].custo;
        resultado.beneficio += itens[i].beneficio;
        if (regiao->quantidade > resultado.maior_quantidade)
            resultado.maior_quantidade = regiao->quantidade;
    }
    qsort(resultado.regioes, resultado.quantidade_regioes,
          sizeof *resultado.regioes, resumo_comparar_regiao);
    *resumo = resultado;
    return 1;

erro:
    resumo_liberar(&resultado);
    return 0;
}

void resumo_mostrar(const ResumoPlano *resumo)
{
    if (resumo == NULL)
        return;
    printf("\n=== MODULO 9: RESUMO ESTRATEGICO POR REGIAO ===\n");
    if (resumo->selecionadas == 0)
        printf("Nenhuma ocorrencia selecionada para resumir.\n");
    for (int i = 0; i < resumo->quantidade_regioes; i++)
    {
        const ResumoRegiao *regiao = &resumo->regioes[i];
        printf("Regiao: %s | Selecionadas: %d | Tempo: %d min | Beneficio: %d pontos\n",
               regiao->regiao[0] ? regiao->regiao : "(sem regiao)",
               regiao->quantidade, regiao->minutos, regiao->beneficio);
    }
    printf("Total do resumo: %d ocorrencia(s) | %d min | %d pontos\n",
           resumo->selecionadas, resumo->minutos, resumo->beneficio);
    if (resumo->selecionadas > 0)
    {
        printf("Regiao(oes) com mais atendimentos (%d):\n", resumo->maior_quantidade);
        for (int i = 0; i < resumo->quantidade_regioes; i++)
            if (resumo->regioes[i].quantidade == resumo->maior_quantidade)
                printf("- %s\n", resumo->regioes[i].regiao[0] ?
                       resumo->regioes[i].regiao : "(sem regiao)");
    }
}
