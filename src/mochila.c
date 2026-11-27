// Programacao dinamica: mochila 0/1 com reconstrucao da selecao.
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "mochila.h"

// dp[i][c]: maior beneficio usando os i primeiros itens e c minutos.
int plano_mochila(const ItemPlano *itens, int n, int capacidade, int *escolhido)
{
    if (n < 0 || n > INT_MAX / 4 || capacidade < 0 || capacidade > MAX_MINUTOS)
    {
        return -1;
    }
    for (int i = 0; i < n; i++)
    {
        if (itens[i].custo <= 0 || itens[i].beneficio < 0 || itens[i].beneficio > 4)
        {
            return -1;
        }
        escolhido[i] = 0;
    }

    // Matriz alocada na heap para nao ocupar a pilha.
    int colunas = capacidade + 1;
    int (*dp)[colunas] = NULL;
    if ((size_t)n + 1 > SIZE_MAX / sizeof *dp)
    {
        return -1;
    }
    dp = calloc(n + 1, sizeof *dp);
    if (dp == NULL)
    {
        return -1;
    }

    for (int i = 1; i <= n; i++)
    {
        int custo = itens[i - 1].custo;
        int valor = itens[i - 1].beneficio;

        for (int c = 0; c <= capacidade; c++)
        {
            dp[i][c] = dp[i - 1][c];
            if (custo <= c)
            {
                int incluir = valor + dp[i - 1][c - custo];
                if (incluir > dp[i][c])
                {
                    dp[i][c] = incluir;
                }
            }
        }
    }

    // Volta pela tabela para descobrir quais itens foram escolhidos.
    int c = capacidade;
    int beneficio = dp[n][c];
    for (int i = n; i > 0; i--)
    {
        if (dp[i][c] != dp[i - 1][c])
        {
            escolhido[i - 1] = 1;
            c -= itens[i - 1].custo;
        }
    }
    free(dp);
    return beneficio;
}
