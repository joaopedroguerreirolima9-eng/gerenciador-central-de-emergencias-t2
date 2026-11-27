#ifndef ENTRADA_H
#define ENTRADA_H

// Le uma linha digitada (sem o Enter), guardando no maximo tamanho - 1 letras.
void ler_texto(char *mensagem, char *texto, int tamanho);

// Le inteiro na faixa de int; rejeita entrada truncada ou fora dos limites.
// Repete em erro; devolve 0 quando a entrada termina.
int ler_inteiro(char *mensagem);

// Le um numero com casas decimais (usado nas coordenadas); repete ate ser valido.
double ler_decimal(char *mensagem);

#endif
