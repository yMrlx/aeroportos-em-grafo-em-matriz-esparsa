#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo_aeroportos.h"

#define CAPACIDADE_INICIAL  5
#define FATOR_CRESCIMENTO   2

static void liberarVoo(void *dado) {
    free((Voo *)dado);
}

GrafoAeroportos *criarGrafo(int capacidadeInicial) {
    if (capacidadeInicial <= 0) capacidadeInicial = CAPACIDADE_INICIAL;

    GrafoAeroportos *g = (GrafoAeroportos *)malloc(sizeof(GrafoAeroportos));
    if (!g) return NULL;

    g->aeroportos = (Aeroporto *)malloc(capacidadeInicial * sizeof(Aeroporto));
    if (!g->aeroportos) { free(g); return NULL; }

    g->quantidade = 0;
    g->capacidade = capacidadeInicial;

    g->voos = criarMatrizEsparsa(capacidadeInicial);
    if (!g->voos) {
        free(g->aeroportos);
        free(g);
        return NULL;
    }

    return g;
}

void ApagarGrafo(GrafoAeroportos *g) {
    if (!g) return;
    apagarMatrizEsparsa(g->voos, liberarVoo);
    free(g->aeroportos);
    free(g);
}

int buscarIndice(GrafoAeroportos *g, const char *codigo) {
    if (!g || !codigo) return -1;
    for (int i = 0; i < g->quantidade; i++) {
        if (strcmp(g->aeroportos[i].codigo, codigo) == 0)
            return i;
    }
    return -1;
}

int cadastrarAeroporto(GrafoAeroportos *g, const char *codigo, const char *cidade) {

    if (!g || !codigo || !cidade) {
        printf("Informaçoes invalidas para cadastro de aeroporto!\n");
        return 0;
    }
    if (strlen(codigo) == 0 || strlen(cidade) == 0) {
        printf("Codigo e cidade nao podem ser vazios.\n");
        return 0;
    }

    if (buscarIndice(g, codigo) != -1) {
        printf("Aeroporto com codigo '%s' ja cadastrado.\n", codigo);
        return 0;
    }

    
    if (g->quantidade == g->capacidade) {
        int novaCapacidade = g->capacidade * FATOR_CRESCIMENTO;

        Aeroporto *novoVetor = (Aeroporto *)realloc(
            g->aeroportos, novaCapacidade * sizeof(Aeroporto)
        );
        if (!novoVetor) {
            printf("Falha ao expandir vetor de aeroportos.\n");
            return 0;
        }
        g->aeroportos = novoVetor;
        g->capacidade = novaCapacidade;

        if (!expandirMatrizEsparsa(g->voos, novaCapacidade)) {
            printf("Falha ao expandir matriz esparsa.\n");
            return 0;
        }
    }

    int idx = g->quantidade;
    strncpy(g->aeroportos[idx].codigo, codigo, CODIGO_AEROPORTO_TAM - 1);
    g->aeroportos[idx].codigo[CODIGO_AEROPORTO_TAM - 1] = '\0';

    strncpy(g->aeroportos[idx].cidade, cidade, NOME_CIDADE_TAM - 1);
    g->aeroportos[idx].cidade[NOME_CIDADE_TAM - 1] = '\0';

    g->quantidade++;

    printf("  Aeroporto cadastrado: %s - %s (indice %d)\n", codigo, cidade, idx);
    return 1;
}

int cadastrarVoo(GrafoAeroportos *g, const char *codigoOrigem, const char *codigoDestino, int numeroVoo) {

    if (!g || !codigoOrigem || !codigoDestino) {
        printf("Parametros invalidos para cadastro de voo.\n");
        return 0;
    }
    if (numeroVoo <= 0) {
        printf(" Numero de voo deve ser positivo.\n");
        return 0;
    }

    int idxOrigem  = buscarIndice(g, codigoOrigem);
    int idxDestino = buscarIndice(g, codigoDestino);

    if (idxOrigem == -1) {
        printf(" Aeroporto de origem '%s' nao encontrado.\n", codigoOrigem);
        return 0;
    }
    if (idxDestino == -1) {
        printf(" Aeroporto de destino '%s' nao encontrado.\n", codigoDestino);
        return 0;
    }
    if (idxOrigem == idxDestino) {
        printf(" Origem e destino nao podem ser o mesmo aeroporto.\n");
        return 0;
    }

    for (int i = 0; i < g->quantidade; i++) {
        NoMatriz *no = obterLinhaMatrizEsparsa(g->voos, i);
        while (no) {
            Voo *voo = (Voo *)no->dado;
            if (voo && voo->numero == numeroVoo) {
                printf(" Numero de voo %d ja cadastrado.\n", numeroVoo);
                return 0;
            }
            no = no->prox;
        }
    }

    if (buscarMatrizEsparsa(g->voos, idxOrigem, idxDestino) != NULL) {
        printf(" Ja existe um voo cadastrado de %s para %s.\n",
               codigoOrigem, codigoDestino);
        return 0;
    }

    Voo *novoVoo = (Voo *)malloc(sizeof(Voo));
    if (!novoVoo) {
        printf(" Falha de alocacao de memoria para o voo.\n");
        return 0;
    }
    novoVoo->numero = numeroVoo;

    if (!inserirMatrizEsparsa(g->voos, idxOrigem, idxDestino, novoVoo)) {
        printf(" Falha ao inserir voo na matriz.\n");
        free(novoVoo);
        return 0;
    }

    printf("  Voo %d cadastrado: %s (%s) --> %s (%s)\n", numeroVoo, codigoOrigem, g->aeroportos[idxOrigem].cidade, codigoDestino, g->aeroportos[idxDestino].cidade);
    return 1;
}

/* ═══════════════════════════════════════════════════════════════
 * OPERAÇÃO 3 — GA_removerVoo  [A IMPLEMENTAR]
 * ─────────────────────────────────────────────────────────────
 * DICA DE IMPLEMENTAÇÃO:
 *   - Percorrer todas as linhas da matriz (loop i de 0 a quantidade)
 *   - Em cada linha, percorrer os nós com obterLinhaMatrizEsparsa
 *   - Quando encontrar o nó com voo->numero == numeroVoo:
 *       → removerMatrizEsparsa(g->voos, i, no->coluna)  retorna o Voo*
 *       → free(voo) para liberar a memória
 *       → retornar 1 (sucesso)
 *   - Se não encontrar em nenhuma linha: retornar 0
 * ═══════════════════════════════════════════════════════════════ */
int removerVoo(GrafoAeroportos *g, int numeroVoo) {
    if (!g) return 0;

    for (int i = 0; i < g->quantidade; i++) {
        NoMatriz *no = obterLinhaMatrizEsparsa(g->voos, i);
        while (no) {
            Voo *voo = (Voo *)no->dado;
            if (voo && voo->numero == numeroVoo) {
                int coluna = no->coluna;
                Voo *removido = (Voo *)removerLinhaMatrizEsparsa(g->voos, i, coluna);
                printf("  Voo %d removido: %s --> %s\n", numeroVoo,
                       g->aeroportos[i].codigo,
                       g->aeroportos[coluna].codigo);
                free(removido);
                return 1;
            }
            no = no->prox;
        }
    }

    printf("  Voo %d nao encontrado.\n", numeroVoo);
    return 0;
}

/* ═══════════════════════════════════════════════════════════════
 * OPERAÇÃO 4 — GA_listarVoosDeAeroporto  [A IMPLEMENTAR]
 * ─────────────────────────────────────────────────────────────
 * DICA DE IMPLEMENTAÇÃO:
 *   - GA_buscarIndice para achar o índice do aeroporto
 *   - ME_obterLinha(g->voos, idx) retorna a lista de voos
 *   - Para cada nó na lista:
 *       → no->coluna é o índice do aeroporto destino
 *       → g->aeroportos[no->coluna].cidade é o nome da cidade
 *       → ((Voo*)no->dado)->numero é o número do voo
 * ═══════════════════════════════════════════════════════════════ */
void listarVoosDeAeroporto(GrafoAeroportos *g, const char *codigoOrigem) {
    if (!g || !codigoOrigem) return;

    int idx = buscarIndice(g, codigoOrigem);
    if (idx == -1) {
        printf("Aeroporto '%s' nao encontrado.\n", codigoOrigem);
        return;
    }

    NoMatriz *no = obterLinhaMatrizEsparsa(g->voos, idx);
    if (!no) {
        printf("Nenhum voo saindo de %s.\n", codigoOrigem);
        return;
    }

    printf("  Voos saindo de %s (%s):\n\n", codigoOrigem, g->aeroportos[idx].cidade);
    while (no) {
        Voo *voo = (Voo *)no->dado;
        printf("    Voo %d --> %s (%s)\n",
               voo->numero,
               g->aeroportos[no->coluna].codigo,
               g->aeroportos[no->coluna].cidade);
        no = no->prox;
    }
}

static void dfs(GrafoAeroportos *g, int atual, int destino,
                int *visitado, int *caminho, int profundidade) {
    visitado[atual] = 1;
    caminho[profundidade] = atual;

    if (atual == destino) {
        printf("    ");
        for (int i = 0; i <= profundidade; i++) {
            printf("%s", g->aeroportos[caminho[i]].codigo);
            if (i < profundidade) printf(" --> ");
        }
        printf("\n");
    } else {
        NoMatriz *no = obterLinhaMatrizEsparsa(g->voos, atual);
        while (no) {
            if (!visitado[no->coluna]) {
                dfs(g, no->coluna, destino, visitado, caminho, profundidade + 1);
            }
            no = no->prox;
        }
    }

    visitado[atual] = 0;
}

void listarTrajetos(GrafoAeroportos *g,
                    const char *codigoOrigem,
                    const char *codigoDestino) {
    if (!g || !codigoOrigem || !codigoDestino) return;

    int idxOrigem  = buscarIndice(g, codigoOrigem);
    int idxDestino = buscarIndice(g, codigoDestino);

    if (idxOrigem == -1) {
        printf("Aeroporto '%s' nao encontrado.\n", codigoOrigem);
        return;
    }
    if (idxDestino == -1) {
        printf("Aeroporto '%s' nao encontrado.\n", codigoDestino);
        return;
    }

    int visitado[g->quantidade];
    int caminho[g->quantidade];
    for (int i = 0; i < g->quantidade; i++) visitado[i] = 0;

    printf("  Trajetos de %s para %s:\n\n", codigoOrigem, codigoDestino);
    dfs(g, idxOrigem, idxDestino, visitado, caminho, 0);
}

void listarAeroportos(GrafoAeroportos *g) {
    if (!g || g->quantidade == 0) {
        printf("  Nenhum aeroporto cadastrado.\n");
        return;
    }
    printf("  %-5s | %-3s | %s\n", "Indice", "Cod", "Cidade");
    printf("  -------+-----+-----------------------------\n");
    for (int i = 0; i < g->quantidade; i++) {
        printf("  %-6d | %-3s | %s\n",
                i,
                g->aeroportos[i].codigo,
                g->aeroportos[i].cidade);
    }
}
