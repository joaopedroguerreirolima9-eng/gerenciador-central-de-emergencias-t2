#include <stdio.h>

// Para compilar:  gcc -Iinclude src/main.c -o central

#include "csv.c"
#include "hash.c"
#include "btree.c"
#include "indices.c"
#include "entrada.c"
#include "central.c"
#include "consultas.c"
#include "integridade.c"
#include "greedy.c"
#include "grafo.c"
#include "dijkstra.c"
#include "prim.c"
#include "rede.c"
#include "kahn.c"
#include "dependencias.c"
#include "resumo.c"
#include "mochila.c"
#include "otimizacao.c"

#define ARQUIVO_DADOS "data/ocorrencias_911.csv"
#define MAX_OCORRENCIAS 1000

static void investigacao_menu(TabelaHash *tabela, char *arquivo, Dependencias *dependencias)
{
    int opcao;
    do
    {
        puts("\n=== MODO INVESTIGACAO: ANALISE OPERACIONAL ===\n"
             "1 - Investigar integridade dos registros\n"
             "2 - Investigar dependencias e ciclos\n0 - Voltar");
        opcao = ler_inteiro("Opcao: ");
        if (opcao == 1)
            integridade_menu(tabela, arquivo);
        else if (opcao == 2)
            dependencias_menu(dependencias);
        else if (opcao != 0)
            puts("Opcao invalida.");
    } while (opcao != 0);
}

int main(void)
{
    // static: o vetor e grande (cerca de 400 KB) e assim fica fora da pilha da funcao.
    static Ocorrencia lidas[MAX_OCORRENCIAS];
    TabelaHash tabela;
    Indices indices;
    static Dependencias dependencias;
    static Rede rede;

    dependencias_inicializar(&dependencias);

    // Carregar os registros iniciais: le o CSV e guarda cada ocorrencia na tabela hash
    // e nas arvores B+ (indices por tipo, regiao e data).
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
    int rede_status = rede_inicializar(&rede, &tabela);
    if (rede_status == REDE_ERRO)
        puts("Aviso: dados invalidos para a rede. Modo Logistica indisponivel nesta sessao.");
    int proximo_id = n + 1; // os ids do arquivo vao de 1 a n
    int opcao = -1;

    while (opcao != 0)
    {
        printf("\n=== CENTRAL DE EMERGENCIAS: OPERACAO RESGATE ===\n");
        printf("1 - Central de Ocorrencias\n");
        printf("2 - Consulta Rapida\n");
        printf("3 - Organizacao das Ocorrencias\n");
        printf("4 - Investigacao (Integridade e Dependencias)\n");
        printf("5 - Operacao Resgate\n");
        printf("6 - Planejamento Operacional\n");
        printf("7 - Planejamento Estrategico\n");
        printf("8 - Logistica (Rede de Emergencias)\n");
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
            investigacao_menu(&tabela, ARQUIVO_DADOS, &dependencias);
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
        else if (opcao == 8)
        {
            if (rede_status == REDE_ERRO)
                puts("Logistica indisponivel: a rede nao foi inicializada com dados validos.");
            else
                rede_menu(&rede);
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
