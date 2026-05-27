#include "matriz_esparsa.h"

MatrizEsparsa *criarMatrizEsparsa(int tamanhoInicial) {
    if (tamanhoInicial < 0) return NULL;

    MatrizEsparsa *m = (MatrizEsparsa *)malloc(sizeof(MatrizEsparsa));
    if (!m) return NULL;

    m->linhas = (NoMatriz **)calloc(tamanhoInicial, sizeof(NoMatriz *));
    if (!m->linhas && tamanhoInicial > 0) {
        free(m);
        return NULL;
    }

    m->tamanho = tamanhoInicial;
    return m;
}

void apagarMatrizEsparsa(MatrizEsparsa *m, void (*liberarDado)(void *)) {
    if (!m) return;

    for (int i = 0; i < m->tamanho; i++) {
        NoMatriz *atual = m->linhas[i];
        while (atual) {
            NoMatriz *prox = atual->prox;
            if (liberarDado && atual->dado) {
                liberarDado(atual->dado);
            }
            free(atual);
            atual = prox;
        }
    }

    free(m->linhas);
    free(m);
}

int expandirMatrizEsparsa(MatrizEsparsa *m, int novoTamanho) {
    if (!m || novoTamanho <= m->tamanho) return 0;

    NoMatriz **novasLinhas = (NoMatriz **)realloc(
        m->linhas, novoTamanho * sizeof(NoMatriz *)
    );
    if (!novasLinhas) return 0;

    for (int i = m->tamanho; i < novoTamanho; i++) {
        novasLinhas[i] = NULL;
    }

    m->linhas  = novasLinhas;
    m->tamanho = novoTamanho;
    return 1;
}

int inserirMatrizEsparsa(MatrizEsparsa *m, int linha, int coluna, void *dado) {
    if (!m || linha < 0 || linha >= m->tamanho || coluna < 0) return 0;
 
    NoMatriz *atual = m->linhas[linha];
    while (atual) {
        if (atual->coluna == coluna) { 
            atual->dado = dado;
            return 1;
        }
        atual = atual->prox;
    }
 
    NoMatriz *novo = (NoMatriz *)malloc(sizeof(NoMatriz));
    if (!novo) return 0;

    novo->coluna = coluna;
    novo->dado   = dado;
 
    novo->prox       = m->linhas[linha];
    m->linhas[linha] = novo;

    return 1;
}
 
void *buscarMatrizEsparsa(MatrizEsparsa *m, int linha, int coluna) {
    if (!m || linha < 0 || linha >= m->tamanho) return NULL;

    NoMatriz *atual = m->linhas[linha];
    while (atual) {
        if (atual->coluna == coluna) return atual->dado;
        atual = atual->prox;
    }

    return NULL; 
}
 
void *removerLinhaMatrizEsparsa(MatrizEsparsa *m, int linha, int coluna) {
    if (!m || linha < 0 || linha >= m->tamanho) return NULL;

    NoMatriz *anterior = NULL;
    NoMatriz *atual    = m->linhas[linha];

    while (atual) {
        if (atual->coluna == coluna) { 
            if (anterior) {
                anterior->prox = atual->prox;
            } else { 
                m->linhas[linha] = atual->prox;
            }

            void *dado = atual->dado;
            free(atual); 
            return dado; 
        }
        anterior = atual;
        atual    = atual->prox;
    }

    return NULL; 
}
 
NoMatriz *obterLinhaMatrizEsparsa(MatrizEsparsa *m, int linha) {
    if (!m || linha < 0 || linha >= m->tamanho) return NULL;
    return m->linhas[linha];
}
