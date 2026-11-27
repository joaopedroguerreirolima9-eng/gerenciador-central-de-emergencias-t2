#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rede.h"
#include "greedy.h"
#include "entrada.h"

static int rede_coordenadas_validas(double lat, double lng)
{
    return isfinite(lat) && isfinite(lng) &&
           lat >= -90.0 && lat <= 90.0 && lng >= -180.0 && lng <= 180.0;
}

void rede_limpar(Rede *rede)
{
    if (rede == NULL)
        return;
    memset(rede, 0, sizeof *rede);
    for (int i = 0; i < MAX_PONTOS_REDE; i++)
        for (int j = 0; j < MAX_PONTOS_REDE; j++)
            rede->custos[i][j] = REDE_SEM_ARESTA;
}

int rede_adicionar(Rede *rede, const char *regiao, double lat, double lng)
{
    if (rede == NULL || regiao == NULL || !regiao[0] ||
        strlen(regiao) >= TAM_REGIAO || !rede_coordenadas_validas(lat, lng) ||
        rede->quantidade < 0 || rede->quantidade >= MAX_PONTOS_REDE)
        return REDE_ERRO;

    for (int i = 0; i < rede->quantidade; i++)
        if (strcmp(rede->pontos[i].regiao, regiao) == 0)
            return REDE_ERRO;

    int indice = rede->quantidade;
    strcpy(rede->pontos[indice].regiao, regiao);
    rede->pontos[indice].lat = lat;
    rede->pontos[indice].lng = lng;
    rede->quantidade++;
    return indice;
}

int rede_ligar(Rede *rede, int origem, int destino, int custo)
{
    if (rede == NULL || rede->quantidade < 0 || rede->quantidade > MAX_PONTOS_REDE ||
        origem < 0 || destino < 0 || origem >= rede->quantidade ||
        destino >= rede->quantidade || origem == destino ||
        custo < 0 || custo > REDE_CUSTO_MAXIMO ||
        rede->custos[origem][destino] != REDE_SEM_ARESTA)
        return 0;

    rede->custos[origem][destino] = custo;
    rede->custos[destino][origem] = custo;
    return 1;
}

static int rede_comparar_regiao(const void *a, const void *b)
{
    const PontoRede *x = a;
    const PontoRede *y = b;
    return strcmp(x->regiao, y->regiao);
}

int rede_inicializar(Rede *rede, const TabelaHash *tabela)
{
    if (rede == NULL)
        return REDE_ERRO;
    rede_limpar(rede);
    if (tabela == NULL)
        return REDE_ERRO;

    int contagens[MAX_PONTOS_REDE] = {0};
    for (int pos = 0; pos < TAM_TABELA; pos++)
    {
        for (const No *no = tabela->posicoes[pos]; no != NULL; no = no->proximo)
        {
            const Ocorrencia *o = &no->ocorrencia;
            if (!o->regiao[0] || !rede_coordenadas_validas(o->lat, o->lng))
                goto erro;

            int indice = 0;
            while (indice < rede->quantidade &&
                   strcmp(rede->pontos[indice].regiao, o->regiao) != 0)
                indice++;
            if (indice == rede->quantidade)
            {
                indice = rede_adicionar(rede, o->regiao, 0.0, 0.0);
                if (indice == REDE_ERRO)
                    goto erro;
            }
            if (contagens[indice] == INT_MAX)
                goto erro;
            rede->pontos[indice].lat += o->lat;
            rede->pontos[indice].lng += o->lng;
            contagens[indice]++;
        }
    }

    for (int i = 0; i < rede->quantidade; i++)
    {
        rede->pontos[i].lat /= contagens[i];
        rede->pontos[i].lng /= contagens[i];
    }
    // Ordena antes de gerar arestas para estabilizar os indices regionais.
    qsort(rede->pontos, rede->quantidade, sizeof *rede->pontos, rede_comparar_regiao);
    for (int i = 0; i < rede->quantidade; i++)
    {
        for (int passo = 1; passo <= 2; passo++)
        {
            int j = (i + passo) % rede->quantidade;
            if (i == j || rede->custos[i][j] != REDE_SEM_ARESTA)
                continue;
            const PontoRede *a = &rede->pontos[i];
            const PontoRede *b = &rede->pontos[j];
            double custo = distancia_km(a->lat, a->lng, b->lat, b->lng) * 100.0;
            if (!isfinite(custo) || custo < 0.0 ||
                custo + 0.5 >= (double)REDE_CUSTO_MAXIMO + 1.0 ||
                !rede_ligar(rede, i, j, (int)(custo + 0.5)))
                goto erro;
        }
    }
    return rede->quantidade;

erro:
    rede_limpar(rede);
    return REDE_ERRO;
}

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
    int opcao;
    do
    {
        puts("\n=== MODO LOGISTICA: REDE DE EMERGENCIAS ===");
        printf("Rede simulada: %d regioes, %d conexoes. Custos em km.\n",
               rede->quantidade, rede_contar_conexoes(rede));
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
