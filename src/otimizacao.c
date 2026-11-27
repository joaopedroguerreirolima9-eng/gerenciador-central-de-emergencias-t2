// Planejamento Estrategico: prepara os itens, executa a mochila e exibe o resumo.
#include <stdio.h>
#include <stdlib.h>

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
        printf("1 - Otimizar recursos e gerar resumo regional\n");
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
