#ifndef MATRIZ_ESPARSA_H
#define MATRIZ_ESPARSA_H

#include <stdio.h>
#include <stdlib.h>

typedef struct NoMatriz {
    int coluna;
    void *dado;
    struct NoMatriz *prox;
} NoMatriz;

typedef struct {
    NoMatriz **linhas;
    int tamanho;
} MatrizEsparsa;

MatrizEsparsa *criarMatrizEsparsa(int tamanhoInicial);

void apagarMatrizEsparsa(MatrizEsparsa *m, void (*liberarDado)(void *));

int expandirMatrizEsparsa(MatrizEsparsa *m, int novoTamanho);

int inserirMatrizEsparsa(MatrizEsparsa *m, int linha, int coluna, void *dado);

void *buscarMatrizEsparsa(MatrizEsparsa *m, int linha, int coluna);

void *removerLinhaMatrizEsparsa(MatrizEsparsa *m, int linha, int coluna);

NoMatriz *obterLinhaMatrizEsparsa(MatrizEsparsa *m, int linha);

#endif
