#ifndef GRAFO_AEROPORTOS_H
#define GRAFO_AEROPORTOS_H

#include "matriz_esparsa.h"

#define CODIGO_AEROPORTO_TAM 4
#define NOME_CIDADE_TAM      64

typedef struct {
    char codigo[CODIGO_AEROPORTO_TAM];
    char cidade[NOME_CIDADE_TAM];  
} Aeroporto;

typedef struct {
    int numero;
} Voo;

typedef struct {
    Aeroporto *aeroportos;  
    int quantidade;  
    int capacidade; 
    MatrizEsparsa *voos;      
} GrafoAeroportos;


GrafoAeroportos *criarGrafo(int capacidadeInicial);

void ApagarGrafo(GrafoAeroportos *g);

int cadastrarAeroporto(GrafoAeroportos *g, const char *codigo, const char *cidade);

int cadastrarVoo(GrafoAeroportos *g, const char *codigoOrigem, const char *codigoDestino, int numeroVoo);

int removerVoo(GrafoAeroportos *g, int numeroVoo);

void listarVoosDeAeroporto(GrafoAeroportos *g, const char *codigoOrigem);

void listarTrajetos(GrafoAeroportos *g, const char *codigoOrigem, const char *codigoDestino);

int buscarIndice(GrafoAeroportos *g, const char *codigo);

void listarAeroportos(GrafoAeroportos *g);

#endif 
