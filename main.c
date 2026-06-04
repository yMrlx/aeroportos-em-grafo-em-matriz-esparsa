#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo_aeroportos.h"

static void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void lerString(const char *prompt, char *destino, int tamanho) {
    printf("%s", prompt);
    if (fgets(destino, tamanho, stdin)) {
        destino[strcspn(destino, "\n")] = '\0';
    }
}

static void menuCadastrarAeroporto(GrafoAeroportos *g) {
    char codigo[CODIGO_AEROPORTO_TAM];
    char cidade[NOME_CIDADE_TAM];
    lerString("  Sigla do aeroporto (ex: GRU): ", codigo, sizeof(codigo));
    lerString("  Nome da cidade:        ", cidade, sizeof(cidade));
    cadastrarAeroporto(g, codigo, cidade);
}

static void menuCadastrarVoo(GrafoAeroportos *g) {
    int idxOrigem, idxDestino, numero;

    printf("\n  Aeroportos cadastrados:\n");
    listarAeroportos(g);

    printf("\n  Indice de origem:  ");
    if (scanf("%d", &idxOrigem) != 1 || idxOrigem < 0 || idxOrigem >= g->quantidade) {
        printf("Indice invalido.\n"); limparBuffer(); return;
    }
    printf("  Indice de destino: ");
    if (scanf("%d", &idxDestino) != 1 || idxDestino < 0 || idxDestino >= g->quantidade) {
        printf("Indice invalido.\n"); limparBuffer(); return;
    }
    printf("  Numero do voo:     ");
    if (scanf("%d", &numero) != 1) {
        printf("Numero invalido.\n"); limparBuffer(); return;
    }
    limparBuffer();
    cadastrarVoo(g, g->aeroportos[idxOrigem].codigo, g->aeroportos[idxDestino].codigo, numero);
}

static void menuRemoverVoo(GrafoAeroportos *g) {
    int numero;
    printf("  Numero do voo a remover: ");
    if (scanf("%d", &numero) != 1) {
        printf("Numero invalido.\n");
        limparBuffer();
        return;
    }
    limparBuffer();
    removerVoo(g, numero); /* Operação 3 — a implementar */
}

static void menuListarVoos(GrafoAeroportos *g) {
    int idx;
    printf("\n  Aeroportos cadastrados:\n");
    listarAeroportos(g);
    printf("\n  Indice do aeroporto de origem: ");
    if (scanf("%d", &idx) != 1 || idx < 0 || idx >= g->quantidade) {
        printf("Indice invalido.\n"); limparBuffer(); return;
    }
    limparBuffer();
    listarVoosDeAeroporto(g, g->aeroportos[idx].codigo);
}

static void menuListarTrajetos(GrafoAeroportos *g) {
    int idxOrigem, idxDestino;
    printf("\n  Aeroportos cadastrados:\n");
    listarAeroportos(g);
    printf("\n  Indice de origem:  ");
    if (scanf("%d", &idxOrigem) != 1 || idxOrigem < 0 || idxOrigem >= g->quantidade) {
        printf("Indice invalido.\n"); limparBuffer(); return;
    }
    printf("  Indice de destino: ");
    if (scanf("%d", &idxDestino) != 1 || idxDestino < 0 || idxDestino >= g->quantidade) {
        printf("Indice invalido.\n"); limparBuffer(); return;
    }
    limparBuffer();
    listarTrajetos(g, g->aeroportos[idxOrigem].codigo, g->aeroportos[idxDestino].codigo);
}

int main(void) {
    GrafoAeroportos *g = criarGrafo(5);
    if (!g) {
        fprintf(stderr, " nao foi possivel criar o grafo.\n");
        return EXIT_FAILURE;
    }

    printf("========================================\n");
    printf("  Carregando dados iniciais...\n");
    printf("========================================\n\n");

    cadastrarAeroporto(g, "BSB", "Brasilia");
    cadastrarAeroporto(g, "CNF", "Belo Horizonte");
    cadastrarAeroporto(g, "GIG", "Rio de Janeiro");
    cadastrarAeroporto(g, "GRU", "Sao Paulo");
    cadastrarAeroporto(g, "SSA", "Salvador");

    printf("\n");

    cadastrarVoo(g, "BSB", "SSA", 107);
    cadastrarVoo(g, "CNF", "GIG", 555);
    cadastrarVoo(g, "CNF", "GRU", 101);
    cadastrarVoo(g, "CNF", "SSA", 214);
    cadastrarVoo(g, "GIG", "CNF", 554);
    cadastrarVoo(g, "GIG", "GRU",  90);
    cadastrarVoo(g, "GRU", "BSB",  50);
    cadastrarVoo(g, "GRU", "CNF", 102);
    cadastrarVoo(g, "GRU", "GIG",  89);
    cadastrarVoo(g, "SSA", "CNF", 215);

    printf("\n========================================\n");
    printf("  Sistema pronto!\n");
    printf("========================================\n\n");

    int opcao;
    do {
        printf("________________________________________\n");
        printf("  1. Cadastrar aeroporto                \n");
        printf("  2. Cadastrar voo                      \n");
        printf("  3. Remover voo                        \n");
        printf("  4. Listar voos de aeroporto           \n");
        printf("  5. Listar trajetos                    \n");
        printf("  6. Listar todos os aeroportos         \n");
        printf("  0. Sair                               \n");
        printf("________________________________________\n");
        printf("Opcao: ");

        if (scanf("%d", &opcao) != 1) {
            limparBuffer();
            opcao = -1;
            printf("\n Entrada invalida, tente novamente.\n\n");
            continue;
        }
        int prox = getchar();
        if (prox != '\n' && prox != EOF) {
            limparBuffer();
            opcao = -1;
            printf("\n Entrada invalida, tente novamente.\n\n");
            continue;
        }
        printf("\n");

        switch (opcao) {
            case 1: menuCadastrarAeroporto(g); break;
            case 2: menuCadastrarVoo(g);       break;
            case 3: menuRemoverVoo(g);         break;
            case 4: menuListarVoos(g);         break;
            case 5: menuListarTrajetos(g);     break;
            case 6:
                printf("  Aeroportos cadastrados:\n\n");
                listarAeroportos(g);
                break;
            case 0:
                printf("Encerrando...\n");
                break;
            default:
                printf(" Opcao invalida!\n");
        }
        printf("\n");
    } while (opcao != 0);

    ApagarGrafo(g);
    return EXIT_SUCCESS;
}
