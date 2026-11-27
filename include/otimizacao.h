#ifndef OTIMIZACAO_H
#define OTIMIZACAO_H
#include "hash.h"
#include "indices.h"
#include "mochila.h"

ItemPlano plano_item(const Ocorrencia *o);
void otimizacao_menu(TabelaHash *tabela, Indices *indices);
void plano_mostrar_dados(const Ocorrencia *o);
void plano_consultar_dados(TabelaHash *tabela);
#endif
