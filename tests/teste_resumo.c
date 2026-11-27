#include <assert.h>

#define main central_main
#include "../src/main.c"
#undef main

static void resumo_teste_base(TabelaHash *tabela)
{
    const char *regioes[] = {"ZETA", "ALFA", "ZETA", "ALFA", "", "BETA"};
    hash_inicializar(tabela);
    for (int i = 0; i < 6; i++)
    {
        Ocorrencia o = {0};
        o.id = i + 1;
        strcpy(o.regiao, regioes[i]);
        strcpy(o.tipo, "EMS: TESTE");
        assert(hash_inserir(tabela, o));
    }
}

static void resumo_conferir_totais(const ResumoPlano *resumo,
                                  const ItemPlano *itens, const int *escolhido, int n)
{
    int quantidade = 0, minutos = 0, beneficio = 0;
    for (int i = 0; i < n; i++)
        if (escolhido[i])
        {
            quantidade++;
            minutos += itens[i].custo;
            beneficio += itens[i].beneficio;
        }
    assert(resumo->selecionadas == quantidade);
    assert(resumo->minutos == minutos && resumo->beneficio == beneficio);
    quantidade = minutos = beneficio = 0;
    int maior = 0;
    for (int i = 0; i < resumo->quantidade_regioes; i++)
    {
        quantidade += resumo->regioes[i].quantidade;
        minutos += resumo->regioes[i].minutos;
        beneficio += resumo->regioes[i].beneficio;
        if (resumo->regioes[i].quantidade > maior)
            maior = resumo->regioes[i].quantidade;
        if (i > 0)
            assert(strcmp(resumo->regioes[i - 1].regiao, resumo->regioes[i].regiao) < 0);
    }
    assert(resumo->selecionadas == quantidade);
    assert(resumo->minutos == minutos && resumo->beneficio == beneficio);
    assert(resumo->maior_quantidade == maior);
}

static void testar_agrupamento(void)
{
    TabelaHash tabela;
    resumo_teste_base(&tabela);
    // Custos deliberadamente diferentes de plano_item: deve preservar os itens recebidos.
    ItemPlano itens[] = {{1, 12, 4}, {2, 18, 3}, {3, 7, 2},
                         {4, 22, 4}, {5, 9, 1}, {6, 30, 4}};
    int escolhido[] = {1, 1, 1, 1, 1, 0};
    ResumoPlano resumo;
    assert(resumo_calcular(&tabela, itens, escolhido, 6, &resumo));
    resumo_conferir_totais(&resumo, itens, escolhido, 6);
    assert(resumo.quantidade_regioes == 3 && resumo.maior_quantidade == 2);
    assert(resumo.selecionadas == 5 && resumo.minutos == 68 && resumo.beneficio == 14);
    assert(resumo.regioes[0].regiao[0] == '\0');
    assert(resumo.regioes[0].quantidade == 1 && resumo.regioes[0].minutos == 9);
    assert(strcmp(resumo.regioes[1].regiao, "ALFA") == 0);
    assert(resumo.regioes[1].quantidade == 2 && resumo.regioes[1].minutos == 40);
    assert(resumo.regioes[1].beneficio == 7);
    assert(strcmp(resumo.regioes[2].regiao, "ZETA") == 0);
    assert(resumo.regioes[2].quantidade == 2 && resumo.regioes[2].minutos == 19);
    assert(resumo.regioes[2].beneficio == 6);
    resumo_mostrar(&resumo); // permite conferir rotulo vazio e todas as lideres
    assert(hash_buscar(&tabela, 1)->tipo[0] == 'E');
    assert(hash_buscar(&tabela, 5)->regiao[0] == '\0');
    resumo_liberar(&resumo);
    assert(resumo.regioes == NULL && resumo.quantidade_regioes == 0);
    resumo_liberar(&resumo); // resultado ja liberado

    memset(escolhido, 0, sizeof escolhido);
    assert(resumo_calcular(&tabela, itens, escolhido, 6, &resumo));
    resumo_conferir_totais(&resumo, itens, escolhido, 6);
    assert(resumo.regioes == NULL && resumo.quantidade_regioes == 0);
    resumo_liberar(&resumo);
    assert(resumo_calcular(&tabela, NULL, NULL, 0, &resumo));
    assert(resumo.selecionadas == 0 && resumo.maior_quantidade == 0);
    resumo_mostrar(&resumo);
    resumo_liberar(&resumo);
    hash_liberar(&tabela);
    puts("OK: multiplas regioes, empate, regiao sem nome, selecao vazia e totais preservados.");
}

static void testar_erros(void)
{
    TabelaHash tabela;
    resumo_teste_base(&tabela);
    ItemPlano itens[] = {{1, 30, 4}, {999, 30, 4}};
    int escolhido[] = {1, 1};
    ResumoPlano resumo;
    assert(!resumo_calcular(&tabela, itens, escolhido, 2, &resumo));
    assert(resumo.regioes == NULL && resumo.selecionadas == 0);
    escolhido[1] = 0; // ID fora da selecao nao deve entrar no relatorio
    assert(resumo_calcular(&tabela, itens, escolhido, 2, &resumo));
    assert(resumo.selecionadas == 1);
    resumo_liberar(&resumo);
    itens[0].id = -1;
    assert(!resumo_calcular(&tabela, itens, escolhido, 2, &resumo));
    itens[0].id = 1;
    itens[0].custo = 0;
    assert(!resumo_calcular(&tabela, itens, escolhido, 2, &resumo));
    itens[0].custo = 30;
    itens[0].beneficio = 5;
    assert(!resumo_calcular(&tabela, itens, escolhido, 2, &resumo));
    itens[0].beneficio = -1;
    assert(!resumo_calcular(&tabela, itens, escolhido, 2, &resumo));
    itens[0].beneficio = 4;
    escolhido[0] = 2;
    assert(!resumo_calcular(&tabela, itens, escolhido, 2, &resumo));
    escolhido[0] = 1;
    assert(!resumo_calcular(NULL, itens, escolhido, 2, &resumo));
    assert(!resumo_calcular(&tabela, NULL, escolhido, 2, &resumo));
    assert(!resumo_calcular(&tabela, itens, NULL, 2, &resumo));
    assert(!resumo_calcular(&tabela, itens, escolhido, -1, &resumo));
    assert(!resumo_calcular(&tabela, itens, escolhido, 2, NULL));
    assert(resumo.regioes == NULL && resumo.quantidade_regioes == 0);
    itens[0].custo = INT_MAX;
    itens[1].id = 2;
    itens[1].custo = 1;
    escolhido[1] = 1;
    assert(!resumo_calcular(&tabela, itens, escolhido, 2, &resumo));
    assert(resumo.regioes == NULL && resumo.minutos == 0);
    hash_liberar(&tabela);
    puts("OK: argumentos invalidos, ID ausente, selecao invalida e protecao da soma.");
}

static void testar_csv(void)
{
    static Ocorrencia lidas[MAX_OCORRENCIAS];
    TabelaHash tabela;
    Indices indices;
    assert(csv_carregar(ARQUIVO_DADOS, lidas, MAX_OCORRENCIAS) == 150);
    hash_inicializar(&tabela);
    indices_inicializar(&indices);
    for (int i = 0; i < 150; i++)
    {
        assert(hash_inserir(&tabela, lidas[i]));
        indices_adicionar(&indices, &lidas[i]);
    }
    int ids[150], escolhido[150], antes[150];
    ItemPlano itens[150];
    int n = btree_buscar_faixa(&indices.regiao, "NORRISTOWN", "NORRISTOWN", ids, 150);
    assert(n == 14);
    for (int i = 0; i < n; i++)
        itens[i] = plano_item(hash_buscar(&tabela, ids[i]));
    qsort(itens, n, sizeof *itens, plano_comparar_id);
    assert(plano_mochila(itens, n, 60, escolhido) == 8);
    memcpy(antes, escolhido, (size_t)n * sizeof *escolhido);
    ResumoPlano resumo;
    assert(resumo_calcular(&tabela, itens, escolhido, n, &resumo));
    resumo_conferir_totais(&resumo, itens, escolhido, n);
    assert(memcmp(antes, escolhido, (size_t)n * sizeof *escolhido) == 0);
    assert(resumo.quantidade_regioes == 1 && resumo.selecionadas == 2);
    assert(strcmp(resumo.regioes[0].regiao, "NORRISTOWN") == 0);
    assert(resumo.minutos == 60 && resumo.beneficio == 8);
    resumo_mostrar(&resumo);
    resumo_liberar(&resumo);

    n = hash_todos_ids(&tabela, ids, 150);
    for (int i = 0; i < n; i++)
        itens[i] = plano_item(hash_buscar(&tabela, ids[i]));
    qsort(itens, n, sizeof *itens, plano_comparar_id);
    for (int capacidade = 0; capacidade <= 300; capacidade += 15)
    {
        int beneficio = plano_mochila(itens, n, capacidade, escolhido);
        assert(beneficio >= 0);
        assert(resumo_calcular(&tabela, itens, escolhido, n, &resumo));
        resumo_conferir_totais(&resumo, itens, escolhido, n);
        assert(resumo.beneficio == beneficio && resumo.minutos <= capacidade);
        resumo_liberar(&resumo);
    }
    indices_liberar(&indices);
    hash_liberar(&tabela);
    puts("OK: NORRISTOWN com 2 selecionadas/60 min/8 pontos e 21 capacidades na base completa.");
}

int main(void)
{
    testar_agrupamento();
    testar_erros();
    testar_csv();
    puts("Todos os testes do resumo passaram.");
    return 0;
}
