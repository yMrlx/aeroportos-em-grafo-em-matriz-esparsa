/*
 * main.c
 *
 * Programa principal — Menu interativo para o sistema ANAC.
 *
 * Compilar com:
 *   gcc -Wall -o aeroportos main.c grafo_aeroportos.c matriz_esparsa.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo_aeroportos.h"

/* Limpa o buffer de entrada após leitura */
static void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Lê uma string do teclado com limite de tamanho */
static void lerString(const char *prompt, char *destino, int tamanho) {
    printf("%s", prompt);
    if (fgets(destino, tamanho, stdin)) {
        /* Remove o '\n' que fgets inclui */
        destino[strcspn(destino, "\n")] = '\0';
    }
}

/* ─── Funções do Menu ─── */

static void menuCadastrarAeroporto(GrafoAeroportos *g) {
    char codigo[CODIGO_AEROPORTO_TAM];
    char cidade[NOME_CIDADE_TAM];
    lerString("  Sigla do aeroporto (ex: GRU): ", codigo, sizeof(codigo));
    lerString("  Nome da cidade:        ", cidade, sizeof(cidade));
    cadastrarAeroporto(g, codigo, cidade);
}

static void menuCadastrarVoo(GrafoAeroportos *g) {
    char origem[CODIGO_AEROPORTO_TAM];
    char destino[CODIGO_AEROPORTO_TAM];
    int  numero;

    printf("\n  Aeroportos cadastrados:\n");
    listarAeroportos(g);

    lerString("\n  Codigo de origem:  ", origem, sizeof(origem));
    lerString("  Codigo de destino: ", destino, sizeof(destino));
    printf("  Numero do voo:     ");
    if (scanf("%d", &numero) != 1) {
        printf("Numero invalido.\n");
        limparBuffer();
        return;
    }
    limparBuffer();
    cadastrarVoo(g, origem, destino, numero);
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
    char codigo[CODIGO_AEROPORTO_TAM];
    printf("\n  Aeroportos cadastrados:\n");
    listarAeroportos(g);
    lerString("\n  Codigo do aeroporto de origem: ", codigo, sizeof(codigo));
    listarVoosDeAeroporto(g, codigo); /* Operação 4 — a implementar */
}

static void menuListarTrajetos(GrafoAeroportos *g) {
    char origem[CODIGO_AEROPORTO_TAM];
    char destino[CODIGO_AEROPORTO_TAM];
    printf("\n  Aeroportos cadastrados:\n");
    listarAeroportos(g);
    lerString("\n  Codigo de origem:  ", origem, sizeof(origem));
    lerString("  Codigo de destino: ", destino, sizeof(destino));
    listarTrajetos(g, origem, destino); /* Operação 5 — a implementar */
}

/* ─── Main ─── */
int main(void) {
    GrafoAeroportos *g = criarGrafo(5); /* capacidade inicial = 5 aeroportos */
    if (!g) {
        fprintf(stderr, " nao foi possivel criar o grafo.\n");
        return EXIT_FAILURE;
    }

    int opcao;
    do {
        printf("    Sistema ANAC — Malha Aerea          \n");
        printf("________________________________________\n");
        printf("  1. Cadastrar aeroporto                \n");
        printf("  2. Cadastrar voo                      \n");
        printf("  3. Remover voo          [a fazer]     \n");
        printf("  4. Listar voos de aeroporto [a fazer] \n");
        printf("  5. Listar trajetos      [a fazer]     \n");
        printf("  6. Listar todos os aeroportos         \n");
        printf("  0. Sair                               \n");
        printf("________________________________________\n");
        printf("Opcao: ");

        if (scanf("%d", &opcao) != 1) {
            limparBuffer();
            opcao = -1;
            printf(" Entrada invalida, tente novamente.\n");
            continue;
        }
        limparBuffer();
        printf("\n");

        switch (opcao) {
            case 1: menuCadastrarAeroporto(g); break;
            case 2: menuCadastrarVoo(g);       break;
            case 3: menuRemoverVoo(g);         break;
            case 4: menuListarVoos(g);         break;
            case 5: menuListarTrajetos(g);     break;
            case 6:
                printf("  Aeroportos cadastrados:\n");
                GA_listarAeroportos(g);
                break;
            case 0:
                printf("Encerrando...\n");
                break;
            default:
                printf(" Opcao invalida!\n");
        }
    } while (opcao != 0);

    ApagarGrafo(g);
    return EXIT_SUCCESS;
}
