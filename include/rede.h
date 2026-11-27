#ifndef REDE_H
#define REDE_H

#include "grafo.h"
#include "dijkstra.h"
#include "prim.h"

// IDs exibidos ao usuario comecam em 1; converte para indice antes da consulta.
// Deve mostrar nomes, dimensoes, caminhos/conexoes e custos em km (custo / 100.0).
void rede_menu(const Rede *rede);

#endif
