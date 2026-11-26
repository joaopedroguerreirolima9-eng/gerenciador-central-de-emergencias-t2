#include <assert.h>

#define main central_main
#include "../src/main.c"
#undef main
#include "../src/rede.c"

static int contar_arestas(const Rede *rede)
{
    int total = 0;
    for (int u = 0; u < rede->quantidade; u++)
    {
        assert(rede->custos[u][u] == REDE_SEM_ARESTA);
        for (int v = u + 1; v < rede->quantidade; v++)
        {
            assert(rede->custos[u][v] == rede->custos[v][u]);
            if (rede->custos[u][v] != REDE_SEM_ARESTA)
            {
                assert(rede->custos[u][v] >= 0);
                total++;
            }
        }
    }
    return total;
}

// Floyd-Warshall como referencia independente para todos os pares.
// long long permite somar sem depender dos limites internos de Dijkstra.
static void conferir_caminhos(const Rede *rede)
{
    long long referencia[MAX_PONTOS_REDE][MAX_PONTOS_REDE];
    const long long infinito = LLONG_MAX / 4;
    int n = rede->quantidade;
    for (int u = 0; u < n; u++)
        for (int v = 0; v < n; v++)
            referencia[u][v] = u == v ? 0 :
                rede->custos[u][v] == REDE_SEM_ARESTA ? infinito : rede->custos[u][v];
    for (int k = 0; k < n; k++)
        for (int u = 0; u < n; u++)
            for (int v = 0; v < n; v++)
                if (referencia[u][k] + referencia[k][v] < referencia[u][v])
                    referencia[u][v] = referencia[u][k] + referencia[k][v];

    for (int u = 0; u < n; u++)
        for (int v = 0; v < n; v++)
        {
            CaminhoRede caminho;
            memset(&caminho, 0x7f, sizeof caminho);
            int resultado = rede_dijkstra(rede, u, v, &caminho);
            if (referencia[u][v] == infinito)
            {
                assert(resultado == REDE_SEM_SOLUCAO);
                assert(caminho.quantidade == 0 && caminho.custo_total == 0);
                continue;
            }
            assert(resultado == REDE_OK);
            assert(caminho.custo_total == referencia[u][v]);
            assert(caminho.quantidade >= 1 && caminho.quantidade <= n);
            assert(caminho.vertices[0] == u);
            assert(caminho.vertices[caminho.quantidade - 1] == v);
            int custo = 0, vistos[MAX_PONTOS_REDE] = {0};
            for (int i = 0; i < caminho.quantidade; i++)
            {
                int atual = caminho.vertices[i];
                assert(atual >= 0 && atual < n && !vistos[atual]);
                vistos[atual] = 1;
                if (i > 0)
                {
                    int peso = rede->custos[caminho.vertices[i - 1]][atual];
                    assert(peso >= 0);
                    custo += peso;
                }
            }
            assert(custo == caminho.custo_total);
        }
}

static int teste_rede_raiz(int *pais, int vertice)
{
    while (pais[vertice] != vertice)
        vertice = pais[vertice];
    return vertice;
}

static int teste_rede_comparar_arestas(const void *a, const void *b)
{
    const ArestaRede *x = a;
    const ArestaRede *y = b;
    return (x->custo > y->custo) - (x->custo < y->custo);
}

// Kruskal e usado apenas como referencia independente nos testes de Prim.
static int custo_kruskal(const Rede *rede)
{
    ArestaRede arestas[MAX_PONTOS_REDE * (MAX_PONTOS_REDE - 1) / 2];
    int pais[MAX_PONTOS_REDE], n = 0, escolhidas = 0, total = 0;
    if (rede->quantidade == 0)
        return -1;
    for (int u = 0; u < rede->quantidade; u++)
    {
        pais[u] = u;
        for (int v = u + 1; v < rede->quantidade; v++)
            if (rede->custos[u][v] != REDE_SEM_ARESTA)
            {
                ArestaRede aresta = {u, v, rede->custos[u][v]};
                arestas[n++] = aresta;
            }
    }
    qsort(arestas, n, sizeof *arestas, teste_rede_comparar_arestas);
    for (int i = 0; i < n; i++)
    {
        int a = teste_rede_raiz(pais, arestas[i].origem);
        int b = teste_rede_raiz(pais, arestas[i].destino);
        if (a != b)
        {
            pais[a] = b;
            escolhidas++;
            total += arestas[i].custo;
        }
    }
    return escolhidas == rede->quantidade - 1 ? total : -1;
}

static void conferir_arvore(const Rede *rede)
{
    ArvoreRede arvore;
    memset(&arvore, 0x7f, sizeof arvore);
    int referencia = custo_kruskal(rede);
    int resultado = rede_prim(rede, &arvore);
    if (referencia == -1)
    {
        assert(resultado == REDE_SEM_SOLUCAO);
        assert(arvore.quantidade == 0 && arvore.custo_total == 0);
        return;
    }
    assert(resultado == REDE_OK);
    assert(arvore.quantidade == rede->quantidade - 1);
    assert(arvore.custo_total == referencia);

    int pais[MAX_PONTOS_REDE], total = 0;
    for (int i = 0; i < rede->quantidade; i++)
        pais[i] = i;
    for (int i = 0; i < arvore.quantidade; i++)
    {
        const ArestaRede *aresta = &arvore.arestas[i];
        assert(aresta->origem >= 0 && aresta->origem < rede->quantidade);
        assert(aresta->destino >= 0 && aresta->destino < rede->quantidade);
        assert(aresta->custo >= 0);
        assert(rede->custos[aresta->origem][aresta->destino] == aresta->custo);
        int a = teste_rede_raiz(pais, aresta->origem);
        int b = teste_rede_raiz(pais, aresta->destino);
        assert(a != b); // incluir esta aresta nao pode criar ciclo
        pais[a] = b;
        total += aresta->custo;
    }
    assert(total == arvore.custo_total);
    for (int i = 0; i < rede->quantidade; i++)
        assert(teste_rede_raiz(pais, i) == teste_rede_raiz(pais, 0));
}

static void testar_prim(void)
{
    Rede rede;
    ArvoreRede arvore;
    rede_limpar(&rede);
    conferir_arvore(&rede); // rede vazia
    assert(rede_adicionar(&rede, "A", 0, 0) == 0);
    conferir_arvore(&rede); // um vertice: arvore valida sem arestas
    assert(rede_adicionar(&rede, "B", 0, 0) == 1);
    assert(rede_adicionar(&rede, "C", 0, 0) == 2);
    assert(rede_ligar(&rede, 0, 1, 4));
    assert(rede_ligar(&rede, 1, 2, 3));
    assert(rede_ligar(&rede, 0, 2, 10));
    conferir_arvore(&rede);
    assert(rede_prim(&rede, &arvore) == REDE_OK && arvore.custo_total == 7);
    assert(arvore.arestas[0].origem == 0 && arvore.arestas[0].destino == 1);
    assert(arvore.arestas[1].origem == 1 && arvore.arestas[1].destino == 2);
    // A quarta regiao torna a rede desconexa apos duas arestas selecionadas.
    assert(rede_adicionar(&rede, "Isolada", 0, 0) == 3);
    conferir_arvore(&rede);

    rede_limpar(&rede);
    for (int i = 0; i < 4; i++)
    {
        char nome[2] = {(char)('A' + i), '\0'};
        assert(rede_adicionar(&rede, nome, 0, 0) == i);
    }
    assert(rede_ligar(&rede, 0, 1, 1));
    assert(rede_ligar(&rede, 0, 2, 1));
    assert(rede_ligar(&rede, 1, 3, 1));
    assert(rede_ligar(&rede, 2, 3, 1));
    conferir_arvore(&rede);
    assert(rede_prim(&rede, &arvore) == REDE_OK && arvore.custo_total == 3);
    assert(arvore.arestas[0].destino == 1 && arvore.arestas[1].destino == 2);
    assert(arvore.arestas[2].origem == 1 && arvore.arestas[2].destino == 3);
    // Reduzir pesos a zero continua produzindo uma arvore sem ciclos.
    rede.custos[0][1] = rede.custos[1][0] = 0;
    rede.custos[1][3] = rede.custos[3][1] = 0;
    conferir_arvore(&rede);
    assert(rede_prim(&rede, &arvore) == REDE_OK && arvore.custo_total == 1);

    assert(rede_prim(NULL, &arvore) == REDE_ERRO);
    assert(arvore.quantidade == 0 && arvore.custo_total == 0);
    assert(rede_prim(&rede, NULL) == REDE_ERRO);
    rede.quantidade = -1;
    assert(rede_prim(&rede, &arvore) == REDE_ERRO);
    rede.quantidade = MAX_PONTOS_REDE + 1;
    assert(rede_prim(&rede, &arvore) == REDE_ERRO);
    puts("OK: Prim em triangulo, rede vazia/desconexa, vertice unico, empates e peso zero.");
}

static void testar_validacoes(void)
{
    Rede rede;
    CaminhoRede caminho;
    rede_limpar(&rede);
    assert(rede_adicionar(&rede, "A", 0, 0) == 0);
    assert(rede_adicionar(&rede, "B", 1, 1) == 1);
    assert(rede_adicionar(&rede, "A", 0, 0) == REDE_ERRO);
    assert(rede_adicionar(&rede, "", 0, 0) == REDE_ERRO);
    char longo[TAM_REGIAO + 1];
    memset(longo, 'X', TAM_REGIAO);
    longo[TAM_REGIAO] = '\0';
    assert(rede_adicionar(&rede, longo, 0, 0) == REDE_ERRO);
    assert(rede_adicionar(&rede, "C", NAN, 0) == REDE_ERRO);
    assert(rede_adicionar(&rede, "C", 0, INFINITY) == REDE_ERRO);
    assert(rede_adicionar(&rede, "C", 91, 0) == REDE_ERRO);
    assert(rede_adicionar(&rede, "C", 0, -181) == REDE_ERRO);
    assert(!rede_ligar(&rede, 0, 0, 1));
    assert(!rede_ligar(&rede, -1, 1, 1));
    assert(!rede_ligar(&rede, 0, 2, 1));
    assert(!rede_ligar(&rede, 0, 1, -1));
    assert(!rede_ligar(&rede, 0, 1, REDE_CUSTO_MAXIMO + 1));
    assert(rede_ligar(&rede, 0, 1, 0));
    assert(!rede_ligar(&rede, 1, 0, 2));
    assert(rede.custos[0][1] == 0 && rede.custos[1][0] == 0);
    assert(rede_dijkstra(&rede, -1, 0, &caminho) == REDE_ERRO);
    assert(caminho.quantidade == 0);
    assert(rede_dijkstra(&rede, 0, 2, &caminho) == REDE_ERRO);
    assert(rede_dijkstra(NULL, 0, 0, &caminho) == REDE_ERRO);
    assert(rede_dijkstra(&rede, 0, 0, NULL) == REDE_ERRO);
    conferir_caminhos(&rede);

    rede_limpar(&rede);
    assert(rede_dijkstra(&rede, 0, 0, &caminho) == REDE_ERRO);
    for (int i = 0; i < MAX_PONTOS_REDE; i++)
    {
        char nome[TAM_REGIAO];
        snprintf(nome, sizeof nome, "Regiao %03d", i);
        assert(rede_adicionar(&rede, nome, 0, 0) == i);
        if (i > 0)
            assert(rede_ligar(&rede, i - 1, i, REDE_CUSTO_MAXIMO));
    }
    assert(rede_adicionar(&rede, "Excesso", 0, 0) == REDE_ERRO);
    assert(rede_dijkstra(&rede, 0, MAX_PONTOS_REDE - 1, &caminho) == REDE_OK);
    assert(caminho.quantidade == MAX_PONTOS_REDE);
    assert(caminho.custo_total == (MAX_PONTOS_REDE - 1) * REDE_CUSTO_MAXIMO);
    conferir_arvore(&rede);
    puts("OK: validacoes, peso zero e caminho de 100 vertices com pesos maximos.");
}

static void testar_grafos_pequenos(void)
{
    Rede rede;
    rede_limpar(&rede);
    const char *nomes[] = {"A", "B", "C", "D", "Isolada"};
    for (int i = 0; i < 5; i++)
        assert(rede_adicionar(&rede, nomes[i], 0, 0) == i);
    assert(rede_ligar(&rede, 0, 1, 4));
    assert(rede_ligar(&rede, 1, 2, 3));
    assert(rede_ligar(&rede, 0, 2, 10));
    assert(rede_ligar(&rede, 2, 3, 0));
    assert(rede_ligar(&rede, 1, 3, 3));
    conferir_caminhos(&rede);
    CaminhoRede caminho;
    assert(rede_dijkstra(&rede, 0, 2, &caminho) == REDE_OK);
    assert(caminho.quantidade == 3 && caminho.custo_total == 7);
    assert(caminho.vertices[1] == 1);
    assert(rede_dijkstra(&rede, 0, 3, &caminho) == REDE_OK);
    assert(caminho.quantidade == 3 && caminho.vertices[1] == 1); // empate preserva B

    unsigned estado = 20261126u;
    for (int caso = 0; caso < 20; caso++)
    {
        rede_limpar(&rede);
        for (int i = 0; i < 8; i++)
        {
            char nome[TAM_REGIAO];
            snprintf(nome, sizeof nome, "Ponto %d", i);
            assert(rede_adicionar(&rede, nome, 0, 0) == i);
        }
        for (int u = 0; u < 8; u++)
            for (int v = u + 1; v < 8; v++)
            {
                estado = estado * 1664525u + 1013904223u;
                if ((estado >> 16) % 3u == 0)
                    assert(rede_ligar(&rede, u, v, (int)((estado >> 8) % 101u)));
            }
        conferir_caminhos(&rede);
        conferir_arvore(&rede);
    }
    puts("OK: triangulo e 20 grafos: Dijkstra versus Floyd-Warshall; Prim versus Kruskal.");
}

static void testar_inicializacao(void)
{
    Rede rede;
    TabelaHash tabela;
    const int esperadas[] = {0, 0, 1, 3, 6, 10};
    for (int n = 0; n <= 5; n++)
    {
        hash_inicializar(&tabela);
        for (int i = n; i > 0; i--)
        {
            Ocorrencia o = {0};
            o.id = i;
            snprintf(o.regiao, sizeof o.regiao, "Regiao %d", i);
            o.lat = i;
            o.lng = -i;
            assert(hash_inserir(&tabela, o));
        }
        assert(rede_inicializar(&rede, &tabela) == n);
        assert(contar_arestas(&rede) == esperadas[n]);
        conferir_caminhos(&rede);
        conferir_arvore(&rede);
        hash_liberar(&tabela);
    }

    hash_inicializar(&tabela);
    Ocorrencia o = {0};
    o.id = 1;
    strcpy(o.regiao, "Media");
    o.lat = 10;
    o.lng = 20;
    assert(hash_inserir(&tabela, o));
    o.id = 2;
    o.lat = 30;
    o.lng = 40;
    assert(hash_inserir(&tabela, o));
    assert(rede_inicializar(&rede, &tabela) == 1);
    assert(rede.pontos[0].lat == 20 && rede.pontos[0].lng == 30);
    assert(hash_buscar(&tabela, 1)->lat == 10); // nao modifica a base
    hash_liberar(&tabela);

    hash_inicializar(&tabela);
    o.id = 1;
    o.regiao[0] = '\0';
    assert(hash_inserir(&tabela, o));
    assert(rede_inicializar(&rede, &tabela) == REDE_ERRO && rede.quantidade == 0);
    strcpy(hash_buscar(&tabela, 1)->regiao, "Invalida");
    hash_buscar(&tabela, 1)->lat = NAN;
    assert(rede_inicializar(&rede, &tabela) == REDE_ERRO && rede.quantidade == 0);
    hash_liberar(&tabela);

    hash_inicializar(&tabela);
    for (int i = 0; i <= MAX_PONTOS_REDE; i++)
    {
        memset(&o, 0, sizeof o);
        o.id = i + 1;
        snprintf(o.regiao, sizeof o.regiao, "Regiao %03d", i);
        assert(hash_inserir(&tabela, o));
    }
    assert(rede_inicializar(&rede, &tabela) == REDE_ERRO && rede.quantidade == 0);
    assert(tabela.quantidade == MAX_PONTOS_REDE + 1);
    hash_liberar(&tabela);
    assert(rede_inicializar(&rede, NULL) == REDE_ERRO && rede.quantidade == 0);
    puts("OK: inicializacao de 0 a 5 regioes, medias, dados invalidos e excesso de regioes.");
}

static void testar_rede_csv(void)
{
    static Ocorrencia lidas[MAX_OCORRENCIAS];
    TabelaHash tabela;
    Rede rede;
    int n = csv_carregar(ARQUIVO_DADOS, lidas, MAX_OCORRENCIAS);
    assert(n == 150);
    hash_inicializar(&tabela);
    for (int i = 0; i < n; i++)
        assert(hash_inserir(&tabela, lidas[i]));
    assert(rede_inicializar(&rede, &tabela) == 41);
    assert(contar_arestas(&rede) == 82);
    for (int i = 1; i < rede.quantidade; i++)
        assert(strcmp(rede.pontos[i - 1].regiao, rede.pontos[i].regiao) < 0);
    assert(strcmp(rede.pontos[0].regiao, "ABINGTON") == 0);
    assert(strcmp(rede.pontos[1].regiao, "AMBLER") == 0);
    assert(rede.custos[0][1] == 1334);
    int menor = INT_MAX, maior = 0;
    for (int u = 0; u < rede.quantidade; u++)
        for (int v = u + 1; v < rede.quantidade; v++)
            if (rede.custos[u][v] != REDE_SEM_ARESTA)
            {
                if (rede.custos[u][v] < menor) menor = rede.custos[u][v];
                if (rede.custos[u][v] > maior) maior = rede.custos[u][v];
            }
    assert(menor == 378 && maior == 6490);
    conferir_caminhos(&rede); // 41 * 41 = 1681 consultas
    CaminhoRede caminho;
    assert(rede_dijkstra(&rede, 0, 21, &caminho) == REDE_OK);
    printf("Exemplo CSV: ");
    for (int i = 0; i < caminho.quantidade; i++)
        printf("%s%s", i ? " -> " : "", rede.pontos[caminho.vertices[i]].regiao);
    printf(" | %.2f km\n", caminho.custo_total / 100.0);
    conferir_arvore(&rede);
    ArvoreRede arvore;
    assert(rede_prim(&rede, &arvore) == REDE_OK && arvore.quantidade == 40);
    assert(arvore.custo_total == 71923);
    printf("Exemplo Prim CSV: %d arestas | %.2f km\n", arvore.quantidade, arvore.custo_total / 100.0);
    hash_liberar(&tabela);
    puts("OK: CSV com 41 vertices/82 arestas; 1681 caminhos versus Floyd-Warshall.");
}

int main(void)
{
    testar_validacoes();
    testar_grafos_pequenos();
    testar_inicializacao();
    testar_rede_csv();
    testar_prim();
    puts("Todos os testes da rede passaram.");
    return 0;
}
