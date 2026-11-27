#include <assert.h>

#define main central_main
#include "../src/main.c"
#undef main

#define REG_TOTAL 160

static char *reg_chave(Ocorrencia *o, int campo)
{
    return campo == 0 ? o->tipo : campo == 1 ? o->regiao : o->data_hora;
}

// Modelo de referencia em vetor, independente da Hash e dos indices B+.
static void reg_conferir(TabelaHash *tabela, Indices *indices,
                        Ocorrencia *modelo, const int *ativo)
{
    int total = 0;
    ArvoreBMais *arvores[] = {&indices->tipo, &indices->regiao, &indices->data};
    for (int i = 0; i < REG_TOTAL; i++)
    {
        Ocorrencia *o = hash_buscar(tabela, modelo[i].id);
        assert((o != NULL) == ativo[i]);
        if (ativo[i])
        {
            total++;
            assert(strcmp(o->tipo, modelo[i].tipo) == 0);
            assert(strcmp(o->regiao, modelo[i].regiao) == 0);
            assert(strcmp(o->data_hora, modelo[i].data_hora) == 0);
            assert(hash_foi_alterada(tabela, o->id) == 0);
        }
        for (int campo = 0; campo < 3; campo++)
        {
            int ids[REG_TOTAL], esperados[REG_TOTAL], quantidade = 0;
            char *chave = reg_chave(&modelo[i], campo);
            for (int j = 0; j < REG_TOTAL; j++)
                if (ativo[j] && strcmp(reg_chave(&modelo[j], campo), chave) == 0)
                    esperados[quantidade++] = modelo[j].id;
            int n = btree_buscar_faixa(arvores[campo], chave, chave, ids, REG_TOTAL);
            assert(n == quantidade);
            assert(memcmp(ids, esperados, (size_t)n * sizeof *ids) == 0);
        }
    }
    assert(tabela->quantidade == total);
    int ids[REG_TOTAL];
    assert(hash_todos_ids(tabela, ids, REG_TOTAL) == total);
    for (int campo = 0; campo < 3; campo++)
    {
        assert(arvores[campo]->quantidade == total);
        int n = btree_buscar_prefixo(arvores[campo], "", ids, REG_TOTAL);
        assert(n == total);
        int vistos[REG_TOTAL] = {0};
        for (int i = 0; i < n; i++)
        {
            int pos = (ids[i] - 1) / TAM_TABELA;
            assert(pos >= 0 && pos < REG_TOTAL && ativo[pos] && !vistos[pos]);
            assert(ids[i] == modelo[pos].id);
            vistos[pos] = 1;
            if (i > 0)
            {
                int anterior = (ids[i - 1] - 1) / TAM_TABELA;
                int ordem = strcmp(reg_chave(&modelo[anterior], campo), reg_chave(&modelo[pos], campo));
                assert(ordem < 0 || (ordem == 0 && ids[i - 1] < ids[i]));
            }
        }
    }
    int esperados = 0;
    for (int i = 0; i < REG_TOTAL; i++)
        if (ativo[i] && strncmp(modelo[i].tipo, "EMS", 3) == 0)
            esperados++;
    assert(btree_buscar_prefixo(&indices->tipo, "EMS", ids, REG_TOTAL) == esperados);
    esperados = 0;
    for (int i = 0; i < REG_TOTAL; i++)
        if (ativo[i] && strcmp(modelo[i].data_hora, "2026-11-10 00:00:00") >= 0 &&
            strcmp(modelo[i].data_hora, "2026-11-20 23:59:59") <= 0)
            esperados++;
    assert(btree_buscar_faixa(&indices->data, "2026-11-10 00:00:00",
                             "2026-11-20 23:59:59", ids, REG_TOTAL) == esperados);
}

static void testar_hash_e_indices(void)
{
    TabelaHash tabela;
    Indices indices;
    Ocorrencia modelo[REG_TOTAL] = {0};
    int ativo[REG_TOTAL] = {0};
    const char *tipos[] = {"EMS: TESTE", "Fire: TESTE", "Traffic: TESTE", "Outras: TESTE"};
    hash_inicializar(&tabela);
    indices_inicializar(&indices);
    for (int i = 0; i < REG_TOTAL; i++)
    {
        // Todos os IDs caem na mesma posicao, exercitando o encadeamento.
        modelo[i].id = 1 + i * TAM_TABELA;
        strcpy(modelo[i].tipo, tipos[i % 4]);
        snprintf(modelo[i].regiao, TAM_REGIAO, "REGIAO_%02d", i % 11);
        snprintf(modelo[i].data_hora, TAM_DATA, "2026-11-%02d %02d:00:00", i % 27 + 1, i % 24);
        modelo[i].lat = 40.1;
        modelo[i].lng = -75.3;
        assert(hash_inserir(&tabela, modelo[i]));
        assert(!hash_inserir(&tabela, modelo[i]));
        indices_adicionar(&indices, &modelo[i]);
        ativo[i] = 1;
    }
    reg_conferir(&tabela, &indices, modelo, ativo);
    for (int i = 0; i < REG_TOTAL; i += 4)
    {
        Ocorrencia *o = hash_buscar(&tabela, modelo[i].id);
        indices_remover(&indices, o);
        strcpy(modelo[i].tipo, "Fire: ALTERADA");
        snprintf(modelo[i].regiao, TAM_REGIAO, "NOVA_%02d", i % 7);
        strcpy(modelo[i].data_hora, "2027-01-01 00:00:00");
        *o = modelo[i];
        indices_adicionar(&indices, o);
        assert(hash_registrar_alteracao(&tabela, o->id));
    }
    reg_conferir(&tabela, &indices, modelo, ativo);
    for (int i = 0; i < REG_TOTAL; i += 3)
    {
        indices_remover(&indices, &modelo[i]);
        assert(hash_remover(&tabela, modelo[i].id));
        assert(!hash_remover(&tabela, modelo[i].id));
        ativo[i] = 0;
    }
    reg_conferir(&tabela, &indices, modelo, ativo);
    for (int i = 0; i < REG_TOTAL; i += 3)
    {
        assert(hash_inserir(&tabela, modelo[i]));
        indices_adicionar(&indices, &modelo[i]);
        ativo[i] = 1;
    }
    reg_conferir(&tabela, &indices, modelo, ativo);
    Ocorrencia *o = hash_buscar(&tabela, modelo[0].id);
    strcpy(o->descricao, "Mudanca sem registro");
    assert(hash_foi_alterada(&tabela, o->id) == 1);
    strcpy(o->descricao, modelo[0].descricao);
    assert(hash_foi_alterada(&tabela, o->id) == 0);
    for (int i = 0; i < REG_TOTAL; i++)
    {
        indices_remover(&indices, &modelo[i]);
        assert(hash_remover(&tabela, modelo[i].id));
        ativo[i] = 0;
    }
    reg_conferir(&tabela, &indices, modelo, ativo);
    assert(hash_inserir(&tabela, modelo[0]));
    indices_adicionar(&indices, &modelo[0]);
    ativo[0] = 1;
    reg_conferir(&tabela, &indices, modelo, ativo);
    indices_liberar(&indices);
    hash_liberar(&tabela);
    assert(tabela.quantidade == 0 && indices.regiao.raiz == NULL);
    puts("OK: 160 colisoes na Hash, indices B+, alteracao, remocao, reinsercao e integridade.");
}

static void testar_reuso_do_guloso(void)
{
    const char *tipos[] = {"EMS: TESTE", "Fire: TESTE", "Traffic: TESTE", "Outra"};
    const int tempos[] = {30, 45, 20, 15};
    for (int i = 0; i < 4; i++)
    {
        Ocorrencia o = {0};
        o.id = i + 1;
        strcpy(o.tipo, tipos[i]);
        assert(prioridade_categoria(&o) == i + 1);
        ItemPlano item = plano_item(&o);
        assert(item.custo == tempos[i] && item.beneficio == 4 - i);
    }
    assert(distancia_km(40, -75, 40, -75) == 0);
    assert(distancia_km(40, -75, 41, -74) == 196);
    assert(distancia_km(41, -74, 40, -75) == 196);
    puts("OK: prioridades e distancia do T1 reutilizadas pela mochila e pelo grafo.");
}

int main(void)
{
    testar_hash_e_indices();
    testar_reuso_do_guloso();
    puts("Todos os testes de regressao passaram.");
    return 0;
}
