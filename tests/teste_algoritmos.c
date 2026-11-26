#include <assert.h>

// Reutiliza a mesma unidade de compilacao do programa, sem iniciar o menu.
#define main central_main
#include "../src/main.c"
#undef main

static void conferir_ordem(const Dependencias *g)
{
    int ordem[MAX_ATIVIDADES], grau[MAX_ATIVIDADES];
    int posicao[MAX_ATIVIDADES];
    int visto[MAX_ATIVIDADES] = {0};
    assert(dependencias_kahn(g, ordem, grau) == g->quantidade);
    for (int i = 0; i < g->quantidade; i++)
    {
        assert(ordem[i] >= 0 && ordem[i] < g->quantidade);
        assert(!visto[ordem[i]]);
        visto[ordem[i]] = 1;
        posicao[ordem[i]] = i;
    }
    for (int u = 0; u < g->quantidade; u++)
        for (int v = 0; v < g->quantidade; v++)
            if (g->arestas[u][v])
                assert(posicao[u] < posicao[v]);
}

static void testar_dependencias(void)
{
    Dependencias g;
    int ordem[MAX_ATIVIDADES], grau[MAX_ATIVIDADES];
    dependencias_inicializar(&g);
    int arestas = 0;
    for (int u = 0; u < g.quantidade; u++)
        for (int v = 0; v < g.quantidade; v++)
            arestas += g.arestas[u][v];
    assert(g.quantidade == 30 && arestas == 54);
    conferir_ordem(&g);
    assert(!dependencias_ligar(&g, 0, 5)); // duplicata
    assert(!dependencias_ligar(&g, -1, 0));
    assert(!dependencias_ligar(&g, 0, g.quantidade));
    assert(dependencias_ligar(&g, 29, 0));
    assert(dependencias_kahn(&g, ordem, grau) < g.quantidade);
    g.arestas[29][0] = 0;
    conferir_ordem(&g);
    assert(dependencias_ligar(&g, 0, 0)); // laco
    assert(dependencias_kahn(&g, ordem, grau) < g.quantidade);
    g.arestas[0][0] = 0;
    assert(dependencias_adicionar(&g, "Atividade isolada"));
    assert(!dependencias_adicionar(&g, "Atividade isolada"));
    assert(!dependencias_adicionar(&g, ""));
    conferir_ordem(&g);
    memset(&g, 0, sizeof g);
    conferir_ordem(&g); // grafo vazio
    puts("OK: Kahn, 30 atividades/54 arestas, ciclo, laco, IDs e vertice isolado.");
}

// Oraculo independente: enumera os subconjuntos, sem programacao dinamica.
static int beneficio_forca_bruta(const ItemPlano *itens, int n, int capacidade)
{
    int melhor = 0;
    for (unsigned mascara = 0; mascara < (1u << n); mascara++)
    {
        int custo = 0, beneficio = 0;
        for (int i = 0; i < n; i++)
            if (mascara & (1u << i))
            {
                custo += itens[i].custo;
                beneficio += itens[i].beneficio;
            }
        if (custo <= capacidade && beneficio > melhor)
            melhor = beneficio;
    }
    return melhor;
}

static void conferir_mochila(const ItemPlano *itens, int n, int capacidade)
{
    int escolhido[8];
    int obtido = plano_mochila(itens, n, capacidade, escolhido);
    assert(obtido == beneficio_forca_bruta(itens, n, capacidade));
    int custo = 0, beneficio = 0;
    for (int i = 0; i < n; i++)
    {
        assert(escolhido[i] == 0 || escolhido[i] == 1);
        if (escolhido[i])
        {
            custo += itens[i].custo;
            beneficio += itens[i].beneficio;
        }
    }
    assert(custo <= capacidade && beneficio == obtido);
}

static void testar_mochila(void)
{
    ItemPlano itens[8] = {{1, 30, 4}, {2, 45, 3}, {3, 20, 2}};
    int escolhido[8];
    conferir_mochila(itens, 0, 0);
    conferir_mochila(itens, 0, 60);
    conferir_mochila(itens, 3, 0);
    conferir_mochila(itens, 3, 1);
    conferir_mochila(itens, 3, MAX_MINUTOS);
    assert(plano_mochila(itens, 3, -1, escolhido) == -1);
    assert(plano_mochila(itens, 3, MAX_MINUTOS + 1, escolhido) == -1);
    assert(plano_mochila(itens, -1, 60, escolhido) == -1);
    ItemPlano invalido = {1, 0, 4};
    assert(plano_mochila(&invalido, 1, 60, escolhido) == -1);
    invalido.custo = 1;
    invalido.beneficio = -1;
    assert(plano_mochila(&invalido, 1, 60, escolhido) == -1);
    invalido.beneficio = 5;
    assert(plano_mochila(&invalido, 1, 60, escolhido) == -1);
    ItemPlano empate[] = {{1, 30, 4}, {2, 30, 4}};
    assert(plano_mochila(empate, 2, 30, escolhido) == 4);
    assert(escolhido[0] == 1 && escolhido[1] == 0);

    // Gerador de semente fixa para reproduzir os mesmos 200 casos.
    unsigned estado = 20261126u;
    for (int caso = 0; caso < 200; caso++)
    {
        int n = caso % 8 + 1;
        for (int i = 0; i < n; i++)
        {
            estado = estado * 1664525u + 1013904223u;
            itens[i].id = i + 1;
            itens[i].custo = (int)((estado >> 16) % 45u) + 1;
            estado = estado * 1664525u + 1013904223u;
            itens[i].beneficio = (int)((estado >> 16) % 5u);
        }
        conferir_mochila(itens, n, caso % 121);
    }
    puts("OK: mochila, limites, empate e 200 comparacoes com forca bruta e reconstrucao.");
}

static void testar_base_csv(void)
{
    static Ocorrencia lidas[MAX_OCORRENCIAS];
    TabelaHash tabela;
    Indices indices;
    int n = csv_carregar(ARQUIVO_DADOS, lidas, MAX_OCORRENCIAS);
    assert(n == 150);
    hash_inicializar(&tabela);
    indices_inicializar(&indices);
    for (int i = 0; i < n; i++)
    {
        assert(hash_inserir(&tabela, lidas[i]));
        indices_adicionar(&indices, &lidas[i]);
    }
    int ids[MAX_OCORRENCIAS], escolhido[MAX_OCORRENCIAS];
    ItemPlano itens[MAX_OCORRENCIAS];
    n = btree_buscar_faixa(&indices.regiao, "NORRISTOWN", "NORRISTOWN", ids, MAX_OCORRENCIAS);
    assert(n == 14);
    for (int i = 0; i < n; i++)
    {
        Ocorrencia *o = hash_buscar(&tabela, ids[i]);
        assert(o != NULL);
        itens[i] = plano_item(o);
    }
    qsort(itens, n, sizeof *itens, plano_comparar_id);
    assert(plano_mochila(itens, n, 60, escolhido) == 8);
    int total = 0, minutos = 0;
    for (int i = 0; i < n; i++)
        if (escolhido[i])
        {
            assert(itens[i].id == 4 || itens[i].id == 19);
            total++;
            minutos += itens[i].custo;
        }
    assert(total == 2 && minutos == 60);
    indices_liberar(&indices);
    hash_liberar(&tabela);
    puts("OK: CSV com 150 registros; NORRISTOWN: IDs 4 e 19, 60 min, 8 pontos.");
}

int main(void)
{
    testar_dependencias();
    testar_mochila();
    testar_base_csv();
    puts("Todos os testes passaram.");
    return 0;
}
