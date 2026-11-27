// Ordenacao topologica de Kahn: grafo direcionado de atividades e fila.
#include "kahn.h"

// Kahn: comeca pelas atividades que nao dependem de nenhuma outra.
int dependencias_kahn(const Dependencias *g, int *ordem, int *grau)
{
    int fila[MAX_ATIVIDADES];
    int inicio = 0;
    int fim = 0;
    int total = 0;
    int n = g->quantidade;

    for (int v = 0; v < n; v++)
    {
        grau[v] = 0;
        for (int u = 0; u < n; u++)
        {
            grau[v] += g->arestas[u][v];
        }
        if (grau[v] == 0)
        {
            fila[fim] = v;
            fim++;
        }
    }

    while (inicio < fim)
    {
        int u = fila[inicio];
        inicio++;
        ordem[total] = u;
        total++;

        for (int v = 0; v < n; v++)
        {
            if (g->arestas[u][v])
            {
                grau[v]--;
                if (grau[v] == 0)
                {
                    fila[fim] = v;
                    fim++;
                }
            }
        }
    }
    return total;
}
