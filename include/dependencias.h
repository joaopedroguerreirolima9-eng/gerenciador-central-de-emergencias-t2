#ifndef DEPENDENCIAS_H
#define DEPENDENCIAS_H
#define MAX_ATIVIDADES 100
#define TAM_ATIVIDADE 80

typedef struct
{
    int quantidade;
    char nomes[MAX_ATIVIDADES][TAM_ATIVIDADE];
    int arestas[MAX_ATIVIDADES][MAX_ATIVIDADES];
} Dependencias;

void dependencias_inicializar(Dependencias *g);
int dependencias_adicionar(Dependencias *g, const char *nome);
int dependencias_ligar(Dependencias *g, int antes, int depois);
void dependencias_menu(Dependencias *g);
#endif
