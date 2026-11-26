#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#include "central.h"
#include "otimizacao.h"
#include "greedy.h"
#include "entrada.h"
#include "resumo.h"

// O CSV nao informa o tempo de atendimento. Usamos valores simulados.
ItemPlano plano_item(const Ocorrencia *o)
{
    int prioridade = prioridade_categoria(o);
    int tempos[] = {0, 30, 45, 20, 15};
    ItemPlano item = {o->id, tempos[prioridade], 5 - prioridade};
    return item;
}

void plano_mostrar_dados(const Ocorrencia *o)
{
    ItemPlano item = plano_item(o);
    printf("Tempo estimado (simulado): %d min | Beneficio (simulado): %d\n",
           item.custo, item.beneficio);
}

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

static int plano_comparar_id(const void *a, const void *b)
{
    const ItemPlano *x = a;
    const ItemPlano *y = b;
    if (x->id < y->id)
    {
        return -1;
    }
    if (x->id > y->id)
    {
        return 1;
    }
    return 0;
}

static void plano_listar(TabelaHash *tabela, ItemPlano *itens, int *escolhido, int n, int grupo)
{
    int quantidade = 0;
    for (int i = 0; i < n; i++)
    {
        if (escolhido[i] == grupo)
        {
            Ocorrencia *o = hash_buscar(tabela, itens[i].id);
            printf("ID %d | %s | %s | %d min | %d pontos\n",
                   o->id, o->tipo, o->regiao, itens[i].custo, itens[i].beneficio);
            quantidade++;
        }
    }
    if (quantidade == 0)
    {
        printf("(nenhuma)\n");
    }
}

static void plano_resultado(TabelaHash *tabela, ItemPlano *itens, int *escolhido, int n, int cap)
{
    if (n == 0)
    {
        printf("Nenhuma ocorrencia na regiao informada.\n");
        return;
    }
    qsort(itens, n, sizeof *itens, plano_comparar_id);
    int beneficio = plano_mochila(itens, n, cap, escolhido);
    if (beneficio < 0)
    {
        printf("Nao foi possivel calcular o plano.\n");
        return;
    }

    int minutos = 0;
    int selecionadas = 0;
    for (int i = 0; i < n; i++)
    {
        if (escolhido[i])
        {
            minutos += itens[i].custo;
            selecionadas++;
        }
    }

    printf("\nSelecionadas:\n");
    plano_listar(tabela, itens, escolhido, n, 1);
    printf("\nNao selecionadas (entre as candidatas):\n");
    plano_listar(tabela, itens, escolhido, n, 0);
    printf("\nSelecionadas: %d de %d | Tempo usado: %d/%d min | Restante: %d min | Beneficio maximo: %d\n",
           selecionadas, n, minutos, cap, cap - minutos, beneficio);
    printf("A mochila escolhe o conjunto com mais pontos dentro do tempo disponivel.\n");
    printf("Simulacao academica; nao substitui protocolos oficiais de emergencia.\n");

    ResumoPlano resumo;
    if (resumo_calcular(tabela, itens, escolhido, n, &resumo))
    {
        resumo_mostrar(&resumo);
        resumo_liberar(&resumo);
    }
    else
        printf("Nao foi possivel gerar o resumo regional desta selecao.\n");
}

static void plano_executar(TabelaHash *tabela, Indices *indices)
{
    char regiao[TAM_REGIAO];
    ler_texto("Regiao (Enter = todas): ", regiao, TAM_REGIAO);
    int cap = ler_inteiro("Tempo operacional disponivel em minutos (0 a 1440): ");
    if (cap < 0 || cap > MAX_MINUTOS)
    {
        printf("Capacidade invalida.\n");
        return;
    }

    int max = tabela->quantidade;
    if (max == 0)
    {
        printf("Nenhuma ocorrencia cadastrada.\n");
        return;
    }
    int *ids = malloc((size_t)max * sizeof *ids);
    ItemPlano *itens = malloc((size_t)max * sizeof *itens);
    int *escolhido = malloc((size_t)max * sizeof *escolhido);
    if (ids == NULL || itens == NULL || escolhido == NULL)
    {
        printf("Sem memoria.\n");
    }
    else
    {
        int total;
        if (regiao[0] != '\0')
        {
            total = btree_buscar_faixa(&indices->regiao, regiao, regiao, ids, max);
        }
        else
        {
            total = hash_todos_ids(tabela, ids, max);
        }
        int n = 0;
        for (int i = 0; i < total; i++)
        {
            Ocorrencia *o = hash_buscar(tabela, ids[i]);
            if (o != NULL)
            {
                itens[n] = plano_item(o);
                n++;
            }
        }
        plano_resultado(tabela, itens, escolhido, n, cap);
    }
    free(ids);
    free(itens);
    free(escolhido);
}

void plano_consultar_dados(TabelaHash *tabela)
{
    int id = ler_inteiro("ID da ocorrencia: ");
    Ocorrencia *o = NULL;
    if (id > 0)
    {
        o = hash_buscar(tabela, id);
    }
    if (o == NULL)
    {
        printf("Ocorrencia nao encontrada.\n");
        return;
    }
    central_mostrar_ocorrencia(o);
    plano_mostrar_dados(o);
}

void otimizacao_menu(TabelaHash *tabela, Indices *indices)
{
    int opcao = -1;
    while (opcao != 0)
    {
        printf("\n=== MODO PLANEJAMENTO ESTRATEGICO ===\n");
        printf("1 - Otimizar recursos\n");
        printf("2 - Consultar dados simulados de uma ocorrencia\n");
        printf("0 - Voltar\n");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1)
        {
            plano_executar(tabela, indices);
        }
        else if (opcao == 2)
        {
            plano_consultar_dados(tabela);
        }
        else if (opcao != 0)
        {
            printf("Opcao invalida.\n");
        }
    }
}
