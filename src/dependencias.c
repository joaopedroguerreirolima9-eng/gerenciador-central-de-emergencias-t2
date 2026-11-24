#include <stdio.h>
#include <string.h>
#include "dependencias.h"
#include "entrada.h"

int dependencias_adicionar(Dependencias *g, const char *nome)
{
    if (!nome[0] || strlen(nome) >= TAM_ATIVIDADE || g->quantidade == MAX_ATIVIDADES)
    {
        return 0;
    }
    for (int i = 0; i < g->quantidade; i++)
    {
        if (strcmp(g->nomes[i], nome) == 0)
        {
            return 0;
        }
    }
    strcpy(g->nomes[g->quantidade], nome);
    g->quantidade++;
    return 1;
}

int dependencias_ligar(Dependencias *g, int antes, int depois)
{
    if (antes < 0 || depois < 0 || antes >= g->quantidade || depois >= g->quantidade ||
        g->arestas[antes][depois])
    {
        return 0;
    }
    // Um laco tambem e um ciclo; permitimos cadastra-lo para investigar o erro.
    g->arestas[antes][depois] = 1;
    return 1;
}

void dependencias_inicializar(Dependencias *g)
{
    // Cenario didatico: 6 fases, 5 frentes de trabalho em cada fase.
    const char *nomes[] = {
        "Registrar chamado", "Identificar regiao", "Registrar recursos disponiveis", "Registrar riscos", "Abrir comunicacao",
        "Classificar chamado", "Classificar regiao", "Conferir equipamentos", "Avaliar riscos", "Confirmar comunicacao",
        "Designar equipe", "Definir unidade responsavel", "Reservar equipamentos", "Definir medidas de seguranca", "Avisar unidade",
        "Encaminhar equipe", "Planejar deslocamento", "Transportar equipamentos", "Preparar local seguro", "Confirmar chegada",
        "Executar atendimento", "Coordenar apoio regional", "Utilizar equipamentos", "Monitorar seguranca", "Atualizar central",
        "Registrar atendimento", "Finalizar apoio regional", "Liberar equipamentos", "Encerrar monitoramento", "Encerrar operacao"};
    memset(g, 0, sizeof *g);
    for (int i = 0; i < 30; i++)
        dependencias_adicionar(g, nomes[i]);
    // Cada atividade da fase seguinte depende de duas da fase anterior.
    for (int fase = 0; fase < 5; fase++)
    {
        for (int j = 0; j < 5; j++)
        {
            int destino = (fase + 1) * 5 + j;
            dependencias_ligar(g, fase * 5 + j, destino);
            dependencias_ligar(g, fase * 5 + (j + 1) % 5, destino);
        }
    }
    // Encerramento exige os quatro registros finais de apoio, alem das dependencias anteriores.
    for (int i = 25; i < 29; i++)
        dependencias_ligar(g, i, 29);
}

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

static void dep_listar(const Dependencias *g)
{
    for (int i = 0; i < g->quantidade; i++)
        printf("%2d - %s\n", i + 1, g->nomes[i]);
}

void dependencias_menu(Dependencias *g)
{
    int op;
    do
    {
        printf("\n=== MODULO 6: DEPENDENCIAS DE ATENDIMENTO ===\n"
               "1 - Listar atividades\n2 - Listar dependencias\n3 - Cadastrar atividade\n"
               "4 - Cadastrar dependencia\n5 - Remover dependencia\n"
               "6 - Verificar ciclos e ordem topologica\n7 - Restaurar cenario inicial\n0 - Voltar\n");
        op = ler_inteiro("Opcao: ");
        if (op == 1)
            dep_listar(g);
        else if (op == 2)
        {
            int total = 0;
            for (int u = 0; u < g->quantidade; u++)
                for (int v = 0; v < g->quantidade; v++)
                    if (g->arestas[u][v])
                    {
                        printf("%d %s -> %d %s\n", u + 1, g->nomes[u], v + 1, g->nomes[v]);
                        total++;
                    }
            printf("%d atividades, %d dependencias.\n", g->quantidade, total);
        }
        else if (op == 3)
        {
            char nome[TAM_ATIVIDADE];
            ler_texto("Nome: ", nome, sizeof nome);
            puts(dependencias_adicionar(g, nome) ? "Atividade cadastrada." : "Nome invalido, repetido ou limite atingido.");
        }
        else if (op == 4 || op == 5)
        {
            dep_listar(g);
            int a = ler_inteiro("ID da atividade que deve terminar antes: ");
            int b = ler_inteiro("ID da atividade dependente: ");
            if (a < 1 || a > g->quantidade || b < 1 || b > g->quantidade)
                puts("IDs invalidos.");
            else if (op == 4)
                puts(dependencias_ligar(g, a - 1, b - 1) ? "Dependencia cadastrada." : "Dependencia ja existe.");
            else if (g->arestas[a - 1][b - 1])
            {
                g->arestas[a - 1][b - 1] = 0;
                puts("Dependencia removida.");
            }
            else
                puts("Dependencia inexistente.");
        }
        else if (op == 6)
        {
            int ordem[MAX_ATIVIDADES], grau[MAX_ATIVIDADES];
            int total = dependencias_kahn(g, ordem, grau);
            if (total == g->quantidade)
            {
                puts("Sem ciclos. Ordem valida de execucao:");
                for (int i = 0; i < total; i++)
                    printf("%2d. [%d] %s\n", i + 1, ordem[i] + 1, g->nomes[ordem[i]]);
            }
            else
            {
                printf("Ciclo detectado! Nao existe ordem valida para todas as atividades (%d de %d processadas).\n", total, g->quantidade);
                puts("Atividades bloqueadas (no ciclo ou dependentes dele):");
                for (int i = 0; i < g->quantidade; i++)
                    if (grau[i] > 0)
                        printf("%d - %s\n", i + 1, g->nomes[i]);
            }
        }
        else if (op == 7)
        {
            dependencias_inicializar(g);
            puts("Cenario restaurado.");
        }
        else if (op != 0)
            puts("Opcao invalida.");
    } while (op != 0);
}
