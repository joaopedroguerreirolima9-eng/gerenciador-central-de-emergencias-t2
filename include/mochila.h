#ifndef MOCHILA_H
#define MOCHILA_H

#define MAX_MINUTOS 1440

typedef struct
{
    int id, custo, beneficio;
} ItemPlano;

// Retorna -1 em erro ou o beneficio maximo; escolhido[i] vale 0 ou 1.
int plano_mochila(const ItemPlano *itens, int n, int capacidade, int *escolhido);

#endif
