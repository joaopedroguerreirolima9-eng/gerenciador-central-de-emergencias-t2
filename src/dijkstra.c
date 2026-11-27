// Caminhos minimos com Dijkstra sobre matriz de adjacencia.
#include <string.h>
#include "dijkstra.h"

int rede_dijkstra(const Rede *rede, int origem, int destino, CaminhoRede *caminho)
{
    if (caminho == NULL)
        return REDE_ERRO;
    memset(caminho, 0, sizeof *caminho);
    if (rede == NULL || rede->quantidade < 0 || rede->quantidade > MAX_PONTOS_REDE ||
        origem < 0 || destino < 0 || origem >= rede->quantidade || destino >= rede->quantidade)
        return REDE_ERRO;

    int distancias[MAX_PONTOS_REDE], anterior[MAX_PONTOS_REDE];
    int visitado[MAX_PONTOS_REDE] = {0};
    for (int i = 0; i < rede->quantidade; i++)
    {
        distancias[i] = INT_MAX;
        anterior[i] = -1;
    }
    distancias[origem] = 0;

    for (int passo = 0; passo < rede->quantidade; passo++)
    {
        int u = -1;
        for (int v = 0; v < rede->quantidade; v++)
            if (!visitado[v] && distancias[v] != INT_MAX &&
                (u == -1 || distancias[v] < distancias[u]))
                u = v;
        if (u == -1)
            return REDE_SEM_SOLUCAO;
        visitado[u] = 1;
        if (u == destino)
            break;

        for (int v = 0; v < rede->quantidade; v++)
        {
            int custo = rede->custos[u][v];
            if (visitado[v] || custo == REDE_SEM_ARESTA)
                continue;
            // O construtor limita os pesos; a soma ainda e protegida aqui.
            if (custo < 0 || custo > REDE_CUSTO_MAXIMO || distancias[u] > INT_MAX - 1 - custo)
                return REDE_ERRO;
            int nova = distancias[u] + custo;
            if (nova < distancias[v])
            {
                distancias[v] = nova;
                anterior[v] = u;
            }
        }
    }

    // Reconstrucao reversa, depois inverte para exibir origem -> destino.
    int v = destino;
    while (v != -1)
    {
        caminho->vertices[caminho->quantidade++] = v;
        v = anterior[v];
    }
    for (int i = 0; i < caminho->quantidade / 2; i++)
    {
        int j = caminho->quantidade - 1 - i;
        int temporario = caminho->vertices[i];
        caminho->vertices[i] = caminho->vertices[j];
        caminho->vertices[j] = temporario;
    }
    caminho->custo_total = distancias[destino];
    return REDE_OK;
}
