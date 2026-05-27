### Objetivo do trabalho

Ele tem como objetivo exercitar os conceitos grafos feitos com matriz de adjacência,
conforme foram estudadas em sala de aula. Sua missão é desenvolver para a ANAC
(Agência Nacional de Aviação Civil) um programa para facilitar a organização da malha
aérea do país.

- Criar uma estrutura de dados para cadastrar os aeroportos do país
- Os voos feitos entre os aeroportos. 

### Aeroportos iniciais
A sua estrutura deverá comportar quantos aeroportos forem desejados, porém os abaixo devem estar escrito hardcoded.

- Belo Horizonte (CNF)
- Brasília (BSB)
- Rio de Janeiro (GIG)
- Salvador (SSA)
- São Paulo (GRU)

### Estrutura de Dados (Matriz esparsa dos voos e aeroportos)
| Origem/Destino | 0 BSB | 1 CNF | 2 GIG | 3 GRU | 4 SSA |
|---|---|---|---|---|---|
| **0 BSB** | | | | | 107 |
| **1 CNF** | | | 555 | 101 | 214 |
| **2 GIG** | | 554 | | 090 | |
| **3 GRU** | 050 | 102 | 089 | | |
| **4 SSA** | | 215 | | | |

### Explicação
- Do ponto de vista de uma linha, as interseções com as colunas indicas os destinos.
- Exemplo 1: saindo da linha 1 (CNF) os destinos possíveis são GIG, GRU, SSA
- Exemplo 2: saindo da linha 0 (BSD) o destino possível é apenas SSA
- Exemplo 2: saindo da linha 2 (GIG) os destinos possíveis são CNF e GRU

### Operações
1. Cadastramento de um novo aeroporto;
2. Cadastramento de um voo com um determinado número entre dois aeroportos identificados pelos seus códigos;
3. Remoção de um voo indicado pelo número;
4. Listagem na tela de todos os voos (número e nome da cidade destino) que saem de um determinado aeroporto;
5. Listagem dos possíveis trajetos para, saindo de determinado aeroporto, atingir outro
dado aeroporto (diretamente ou indiretamente, ou seja, passando por outros
aeroportos).

#### 1. Cadastramento de um novo aeroporto

cadastro_aeroporto(char* nome, char* codigo);

#### 2. Cadastramento de um voo com um determinado número entre dois aeroportos identificados pelos seus códigos

cadastro_voo(char* codigoVoo, char* codigoAeroportoPartida, char* codigoAeroportoDestino);

#### 3. Remoção de um voo indicado pelo número

remocao_voo(char* codigoVoo)

#### 4. Listagem na tela de todos os voos (número e nome da cidade destino) que saem de um determinado aeroporto
#### Listagem dos possíveis trajetos para, saindo de determinado aeroporto, atingir outro dado aeroporto (diretamente ou indiretamente, ou seja, passando por outros aeroportos).




### Requisitos
- Menu que permita ao usuário realizar cada uma das operações. Você deverá
fazer vários testes com o seu programa.
- A matriz a ser usada no projeto deverá ser uma **matriz esparsa** implementada conforme **explicada em aula**. 
- Naturalmente, a implementação deverá ser **genérica** (admitir que se guarde qualquer coisa que se deseje guardar) e modular (organizada num .h e num .c)
- Quanto à implementação do vetor, deve-se usar um vetor dinâmico, para poder crescer ou
encolher se for necessário.