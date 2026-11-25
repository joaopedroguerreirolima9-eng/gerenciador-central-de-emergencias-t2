#include <stdio.h>

// Para compilar: gcc -Iinclude src/main.c -o central

#include "csv.c"
#include "hash.c"
#include "btree.c"
#include "indices.c"
#include "entrada.c"
#include "central.c"
#include "consultas.c"
#include "integridade.c"
#include "greedy.c"
#include "dependencias.c"
#include "otimizacao.c"

#define ARQUIVO_DADOS "data/ocorrencias_911.csv"
#define MAX_OCORRENCIAS 1000

static void investigacao_menu(TabelaHash *tabela, Dependencias *g, char *arquivo)
{
    int op;
    do
    {
        printf("\n=== MODO INVESTIGACAO - ANALISE OPERACIONAL ===\n");
        printf("1 - Integridade dos registros (Modulo 4)\n");
        printf("2 - Dependencias e ciclos (Modulo 6)\n");
        printf("0 - Voltar\n");

        op = ler_inteiro("Opcao: ");

        if (op == 1)
            integridade_menu(tabela, arquivo);
        else if (op == 2)
            dependencias_menu(g);
        else if (op != 0)
            puts("Opcao invalida.");

    } while (op != 0);
}

int main(void)
{
    static Ocorrencia lidas[MAX_OCORRENCIAS];
    TabelaHash tabela;
    Indices indices;
    static Dependencias dependencias;

    dependencias_inicializar(&dependencias);

    int n = csv_carregar(ARQUIVO_DADOS, lidas, MAX_OCORRENCIAS);

    if (n == -1)
    {
        printf("Erro: nao foi possivel abrir o arquivo %s\n", ARQUIVO_DADOS);
        return 1;
    }

    hash_inicializar(&tabela);
    indices_inicializar(&indices);

    for (int i = 0; i < n; i++)
    {
        if (hash_inserir(&tabela, lidas[i]))
        {
            indices_adicionar(&indices, &lidas[i]);
        }
    }

    printf("%d ocorrencias carregadas de %s\n", tabela.quantidade, ARQUIVO_DADOS);

    int proximo_id = n + 1;
    int opcao = -1;

    while (opcao != 0)
    {
        printf("\n=== CENTRAL DE EMERGENCIAS: OPERACAO RESGATE ===\n");
        printf("1 - Central de Ocorrencias\n");
        printf("2 - Consulta Rapida\n");
        printf("3 - Organizacao das Ocorrencias\n");
        printf("4 - Investigacao de Registros\n");
        printf("5 - Operacao Resgate (Guloso)\n");
        printf("6 - Planejamento Operacional (Kahn)\n");
        printf("7 - Planejamento Estrategico (Programacao Dinamica)\n");
        printf("0 - Sair\n");

        opcao = ler_inteiro("Opcao: ");

        if (opcao == 1)
        {
            central_menu(&tabela, &indices, &proximo_id);
        }
        else if (opcao == 2)
        {
            consulta_rapida_menu(&tabela, &indices);
        }
        else if (opcao == 3)
        {
            organizacao_menu(&tabela, &indices);
        }
        else if (opcao == 4)
        {
            investigacao_menu(&tabela, &dependencias, ARQUIVO_DADOS);
        }
        else if (opcao == 5)
        {
            greedy_menu(&tabela, &indices);
        }
        else if (opcao == 6)
        {
            dependencias_menu(&dependencias);
        }
        else if (opcao == 7)
        {
            otimizacao_menu(&tabela, &indices);
        }
        else if (opcao != 0)
        {
            printf("Opcao invalida.\n");
        }
    }

    indices_liberar(&indices);
    hash_liberar(&tabela);

    printf("Ate logo!\n");

    return 0;
}