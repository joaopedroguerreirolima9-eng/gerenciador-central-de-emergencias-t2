// Modo Logistica: apresenta o grafo e os resultados de Dijkstra e Prim.
#include <stdio.h>
#include "rede.h"
#include "entrada.h"

static void rede_listar_pontos(const Rede *rede)
{
    if (rede->quantidade == 0)
        puts("Nenhuma regiao na rede.");
    for (int i = 0; i < rede->quantidade; i++)
        printf("%2d - %s\n", i + 1, rede->pontos[i].regiao);
}

static int rede_contar_conexoes(const Rede *rede)
{
    int total = 0;
    for (int u = 0; u < rede->quantidade; u++)
        for (int v = u + 1; v < rede->quantidade; v++)
            if (rede->custos[u][v] != REDE_SEM_ARESTA)
                total++;
    return total;
}

static void rede_listar_conexoes(const Rede *rede)
{
    int total = 0;
    for (int u = 0; u < rede->quantidade; u++)
        for (int v = u + 1; v < rede->quantidade; v++)
            if (rede->custos[u][v] != REDE_SEM_ARESTA)
            {
                printf("[%d] %s -- [%d] %s | %.2f km\n", u + 1, rede->pontos[u].regiao,
                       v + 1, rede->pontos[v].regiao, rede->custos[u][v] / 100.0);
                total++;
            }
    printf("%d conexoes nao direcionadas.\n", total);
}

static void rede_consultar_caminho(const Rede *rede)
{
    if (rede->quantidade == 0)
    {
        puts("Rede vazia; nao ha regioes para consultar.");
        return;
    }
    rede_listar_pontos(rede);
    int origem = ler_inteiro("ID da origem (0 = cancelar): ");
    if (origem == 0)
    {
        puts("Consulta cancelada.");
        return;
    }
    int destino = ler_inteiro("ID do destino (0 = cancelar): ");
    if (destino == 0)
    {
        puts("Consulta cancelada.");
        return;
    }
    if (origem < 1 || destino < 1 || origem > rede->quantidade || destino > rede->quantidade)
    {
        puts("IDs invalidos.");
        return;
    }
    CaminhoRede caminho;
    int resultado = rede_dijkstra(rede, origem - 1, destino - 1, &caminho);
    if (resultado == REDE_SEM_SOLUCAO)
        puts("Destino inalcancavel a partir da origem informada.");
    else if (resultado == REDE_ERRO)
        puts("Nao foi possivel calcular o caminho.");
    else
    {
        puts("Caminho minimo (Dijkstra):");
        for (int i = 0; i < caminho.quantidade; i++)
        {
            int v = caminho.vertices[i];
            printf("%s[%d] %s", i ? " -> " : "", v + 1, rede->pontos[v].regiao);
        }
        printf("\nCusto total do caminho: %.2f km\n", caminho.custo_total / 100.0);
    }
}

static void rede_mostrar_arvore(const Rede *rede)
{
    ArvoreRede arvore;
    int resultado = rede_prim(rede, &arvore);
    if (resultado == REDE_SEM_SOLUCAO)
    {
        puts(rede->quantidade == 0 ? "Rede vazia; nao ha arvore geradora." :
             "Rede desconexa: nao existe arvore geradora para todas as regioes.");
    }
    else if (resultado == REDE_ERRO)
        puts("Nao foi possivel calcular a arvore geradora.");
    else
    {
        puts("Arvore geradora minima (Prim):");
        for (int i = 0; i < arvore.quantidade; i++)
        {
            const ArestaRede *a = &arvore.arestas[i];
            printf("[%d] %s -- [%d] %s | %.2f km\n", a->origem + 1,
                   rede->pontos[a->origem].regiao, a->destino + 1,
                   rede->pontos[a->destino].regiao, a->custo / 100.0);
        }
        printf("Arestas selecionadas: %d | Custo total da arvore: %.2f km\n",
               arvore.quantidade, arvore.custo_total / 100.0);
    }
}

void rede_menu(const Rede *rede)
{
    if (rede == NULL || rede->quantidade < 0 || rede->quantidade > MAX_PONTOS_REDE)
    {
        puts("Rede indisponivel.");
        return;
    }
    int conexoes = rede_contar_conexoes(rede);
    int opcao;
    do
    {
        puts("\n=== MODO LOGISTICA: REDE DE EMERGENCIAS ===");
        printf("Rede simulada: %d regioes, %d conexoes. Custos em km.\n",
               rede->quantidade, conexoes);
        puts("Cenario carregado no inicio da sessao; alteracoes nas ocorrencias nao atualizam esta rede.");
        puts("1 - Listar regioes\n2 - Listar conexoes\n3 - Consultar caminho minimo (Dijkstra)\n"
             "4 - Consultar arvore geradora minima (Prim)\n0 - Voltar");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1)
            rede_listar_pontos(rede);
        else if (opcao == 2)
            rede_listar_conexoes(rede);
        else if (opcao == 3)
            rede_consultar_caminho(rede);
        else if (opcao == 4)
            rede_mostrar_arvore(rede);
        else if (opcao != 0)
            puts("Opcao invalida.");
    } while (opcao != 0);
}
