// Arvore geradora minima com Prim sobre matriz de adjacencia.
#include <string.h>
#include "prim.h"

int rede_prim(const Rede *rede, ArvoreRede *arvore)
{
    if (arvore == NULL)
        return REDE_ERRO;
    memset(arvore, 0, sizeof *arvore);
    if (rede == NULL || rede->quantidade < 0 || rede->quantidade > MAX_PONTOS_REDE)
        return REDE_ERRO;
    if (rede->quantidade == 0)
        return REDE_SEM_SOLUCAO;

    int menor_custo[MAX_PONTOS_REDE], anterior[MAX_PONTOS_REDE];
    int incluido[MAX_PONTOS_REDE] = {0};
    // So publica o resultado depois de conectar todos os vertices.
    ArvoreRede resultado = {0};
    for (int i = 0; i < rede->quantidade; i++)
    {
        menor_custo[i] = INT_MAX;
        anterior[i] = -1;
    }
    menor_custo[0] = 0;

    for (int passo = 0; passo < rede->quantidade; passo++)
    {
        int u = -1;
        for (int v = 0; v < rede->quantidade; v++)
            if (!incluido[v] && menor_custo[v] != INT_MAX &&
                (u == -1 || menor_custo[v] < menor_custo[u]))
                u = v;
        // Ainda ha vertices fora da arvore, mas nenhuma conexao ate eles.
        if (u == -1)
            return REDE_SEM_SOLUCAO;
        incluido[u] = 1;

        if (anterior[u] != -1)
        {
            int custo = menor_custo[u];
            if (resultado.custo_total > INT_MAX - 1 - custo)
                return REDE_ERRO;
            ArestaRede aresta = {anterior[u], u, custo};
            resultado.arestas[resultado.quantidade++] = aresta;
            resultado.custo_total += custo;
        }

        for (int v = 0; v < rede->quantidade; v++)
        {
            int custo = rede->custos[u][v];
            if (incluido[v] || custo == REDE_SEM_ARESTA)
                continue;
            if (custo < 0 || custo > REDE_CUSTO_MAXIMO)
                return REDE_ERRO;
            // Aqui comparamos uma aresta com a arvore, sem somar um percurso.
            if (custo < menor_custo[v])
            {
                menor_custo[v] = custo;
                anterior[v] = u;
            }
        }
    }
    *arvore = resultado;
    return REDE_OK;
}
