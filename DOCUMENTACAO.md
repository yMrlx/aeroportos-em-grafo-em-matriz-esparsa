# Documentação — Sistema de Aeroportos em Grafo com Matriz Esparsa

## Visão Geral

O projeto implementa um sistema de gerenciamento de aeroportos e voos usando um **grafo direcionado** cuja estrutura de adjacência é armazenada em uma **matriz esparsa**. Em vez de alocar uma matriz N×N completa (onde a maioria das células seria vazia), apenas as arestas existentes são armazenadas — economizando memória quando o número de voos é pequeno em relação ao total de combinações possíveis.

---

## Estrutura de Arquivos

```
.
├── matriz_esparsa.h / .c   → estrutura genérica de matriz esparsa
├── grafo_aeroportos.h / .c → grafo de aeroportos construído sobre a matriz
└── main.c                  → interface de menu (entrada e saída do usuário)
```

A dependência é em camadas:

```
main.c
  └── grafo_aeroportos.h/.c
        └── matriz_esparsa.h/.c
```

---

## Módulo: `matriz_esparsa`

### Conceito

Uma **matriz esparsa** é uma matriz onde a maioria dos valores é zero (ou nula). Em vez de alocar N×N células, armazena-se apenas os valores presentes usando listas encadeadas: cada linha da matriz é uma lista de nós, e cada nó guarda a coluna e o dado daquela célula.

Visualmente, para uma matriz 5×5 com poucas entradas:

```
Linha 0: → [col=2, dado] → [col=4, dado] → NULL
Linha 1: → NULL
Linha 2: → [col=1, dado] → NULL
Linha 3: → NULL
Linha 4: → [col=0, dado] → NULL
```

### Estruturas

```c
typedef struct NoMatriz {
    int coluna;          // índice da coluna deste nó
    void *dado;          // ponteiro genérico para qualquer dado
    struct NoMatriz *prox; // próximo nó na mesma linha
} NoMatriz;

typedef struct {
    NoMatriz **linhas;   // vetor de ponteiros; cada posição é o início de uma lista
    int tamanho;         // número de linhas alocadas
} MatrizEsparsa;
```

O campo `void *dado` torna a matriz **genérica**: ela não sabe o que armazena — quem usa decide o tipo (neste projeto, `Voo *`).

### Funções

#### `criarMatrizEsparsa(int tamanhoInicial)`

Aloca a struct `MatrizEsparsa` e o vetor interno `linhas` com `calloc`. O `calloc` (ao contrário do `malloc`) inicializa tudo com zero, garantindo que todos os ponteiros de linha comecem como `NULL` — ou seja, todas as listas começam vazias.

Retorna `NULL` em falha de alocação.

---

#### `apagarMatrizEsparsa(MatrizEsparsa *m, void (*liberarDado)(void *))`

Libera toda a memória da matriz. Percorre linha por linha, e dentro de cada linha percorre os nós da lista encadeada. Para cada nó:

1. Salva o ponteiro para o próximo nó antes de liberar o atual.
2. Chama `liberarDado(atual->dado)` se uma função foi fornecida — assim quem criou os dados decide como liberá-los.
3. Libera o nó em si com `free`.

Por fim, libera o vetor `linhas` e a struct.

```
Antes:  linhas[0] → [A] → [B] → NULL
Depois: tudo liberado
```

---

#### `expandirMatrizEsparsa(MatrizEsparsa *m, int novoTamanho)`

Usa `realloc` para crescer o vetor `linhas`. O `realloc` pode mover o bloco de memória para outro endereço, por isso o resultado é guardado em um ponteiro temporário antes de ser atribuído — se `realloc` falhar, o ponteiro original continua válido.

Após o `realloc`, as novas posições são inicializadas com `NULL` manualmente (o `realloc` não zera a memória nova):

```c
for (int i = m->tamanho; i < novoTamanho; i++)
    novasLinhas[i] = NULL;
```

---

#### `inserirMatrizEsparsa(MatrizEsparsa *m, int linha, int coluna, void *dado)`

Insere ou atualiza um valor na posição `(linha, coluna)`.

Primeiro percorre a lista da linha buscando se já existe um nó com aquela coluna. Se existir, apenas atualiza o dado (sobrescreve). Se não existir, aloca um novo nó e o insere **no início** da lista (inserção no início é O(1)):

```c
novo->prox       = m->linhas[linha];  // novo aponta para o antigo primeiro
m->linhas[linha] = novo;              // novo vira o primeiro
```

---

#### `buscarMatrizEsparsa(MatrizEsparsa *m, int linha, int coluna)`

Percorre a lista da linha procurando um nó com a coluna pedida. Retorna `dado` se encontrar, `NULL` caso contrário. Complexidade O(k), onde k é o número de entradas naquela linha.

---

#### `removerLinhaMatrizEsparsa(MatrizEsparsa *m, int linha, int coluna)`

Remove um nó específico da lista, mantendo a lista encadeada íntegra. Usa dois ponteiros (`anterior` e `atual`) para fazer isso sem perder a cadeia:

```
Antes:  HEAD → [A] → [B] → [C] → NULL
Remover B:
  anterior = A, atual = B
  A->prox = C           ← pula o B
  free(B)
Depois: HEAD → [A] → [C] → NULL
```

Se o nó a remover for o primeiro da lista (`anterior == NULL`), atualiza diretamente `m->linhas[linha]`.

Retorna o `dado` do nó removido para que o chamador possa liberá-lo.

---

#### `obterLinhaMatrizEsparsa(MatrizEsparsa *m, int linha)`

Retorna o ponteiro para o primeiro nó da lista da linha pedida. Permite ao chamador percorrer todos os nós daquela linha diretamente.

---

## Módulo: `grafo_aeroportos`

### Conceito

O grafo representa aeroportos como **vértices** e voos como **arestas direcionadas**. Cada aeroporto tem um índice inteiro (0, 1, 2...), e a matriz esparsa representa a adjacência: a célula `(i, j)` contém um `Voo *` se existe um voo direto do aeroporto `i` para o aeroporto `j`.

```
       BSB CNF GIG GRU SSA
  BSB [  .   .   .   .  107 ]
  CNF [  .   .  555 101 214 ]
  GIG [  .  554  .   90  .  ]
  GRU [  50 102  89  .   .  ]
  SSA [  .  215  .   .   .  ]
```

Apenas as células com voo são armazenadas na matriz esparsa.

### Estruturas

```c
typedef struct {
    char codigo[4];    // sigla IATA (ex: "GRU"), máximo 3 chars + '\0'
    char cidade[64];   // nome da cidade
} Aeroporto;

typedef struct {
    int numero;        // número identificador do voo
} Voo;

typedef struct {
    Aeroporto *aeroportos;  // vetor dinâmico de aeroportos
    int quantidade;         // quantos aeroportos estão cadastrados
    int capacidade;         // tamanho atual do vetor (pode crescer)
    MatrizEsparsa *voos;    // adjacência: voos[i][j] = Voo* ou NULL
} GrafoAeroportos;
```

A relação entre `quantidade` e `capacidade` é importante:
- `quantidade`: aeroportos de fato cadastrados.
- `capacidade`: espaço alocado. Quando `quantidade == capacidade`, o vetor precisa crescer antes de inserir mais um.

### Funções

#### `criarGrafo(int capacidadeInicial)`

Aloca o grafo, o vetor de aeroportos e a matriz esparsa — todos com a mesma `capacidadeInicial`. Se qualquer alocação falhar, libera o que já foi alocado antes de retornar `NULL` (evita vazamento de memória).

---

#### `ApagarGrafo(GrafoAeroportos *g)`

Libera toda a memória do grafo em ordem correta:
1. A matriz esparsa (e todos os `Voo *` dentro dela, via `liberarVoo`).
2. O vetor de aeroportos.
3. A struct do grafo em si.

A função `liberarVoo` é um callback `static` (privado ao arquivo) passado para `apagarMatrizEsparsa`.

---

#### `buscarIndice(GrafoAeroportos *g, const char *codigo)`

Busca linear no vetor de aeroportos comparando códigos com `strcmp`. Retorna o índice inteiro do aeroporto encontrado, ou `-1` se não existir. Esse índice é usado como linha/coluna na matriz esparsa.

---

#### `cadastrarAeroporto(GrafoAeroportos *g, const char *codigo, const char *cidade)`

Valida as entradas e verifica duplicata via `buscarIndice`. Se a capacidade estiver esgotada (`quantidade == capacidade`), dobra o tamanho com `FATOR_CRESCIMENTO = 2` usando `realloc` no vetor de aeroportos e `expandirMatrizEsparsa` na matriz — ambos crescem juntos para manter a correspondência de índices.

Usa `strncpy` com limitação explícita de tamanho para evitar estouro de buffer, e força o terminador `'\0'` na última posição como garantia extra:

```c
strncpy(g->aeroportos[idx].codigo, codigo, CODIGO_AEROPORTO_TAM - 1);
g->aeroportos[idx].codigo[CODIGO_AEROPORTO_TAM - 1] = '\0';
```

---

#### `cadastrarVoo(GrafoAeroportos *g, const char *codigoOrigem, const char *codigoDestino, int numeroVoo)`

Executa várias validações antes de inserir:

1. Ponteiros não nulos e `numeroVoo > 0`.
2. Ambos os aeroportos existem (`buscarIndice != -1`).
3. Origem e destino são diferentes.
4. O número do voo não está em uso (percorre toda a matriz).
5. Já não existe um voo direto entre esse par de aeroportos (`buscarMatrizEsparsa`).

Se tudo passar, aloca um `Voo *` e o insere na posição `(idxOrigem, idxDestino)` da matriz.

---

#### `removerVoo(GrafoAeroportos *g, int numeroVoo)`

Percorre todas as linhas da matriz, e dentro de cada linha percorre os nós. Quando encontra o voo com o número pedido, salva a coluna do nó, remove o nó com `removerLinhaMatrizEsparsa` (que retorna o `Voo *`), e libera a memória. Retorna `1` em sucesso, `0` se não encontrar.

---

#### `listarVoosDeAeroporto(GrafoAeroportos *g, const char *codigoOrigem)`

Obtém a linha do aeroporto de origem na matriz (`obterLinhaMatrizEsparsa`) e percorre a lista de nós. Cada nó representa um voo direto: `no->coluna` é o índice do destino, e `no->dado` é o `Voo *` com o número do voo.

---

#### `dfs` (função estática interna)

Implementa uma **busca em profundidade (DFS — Depth-First Search)** para encontrar todos os caminhos possíveis entre dois aeroportos, incluindo voos com escalas.

Parâmetros relevantes:
- `visitado[]`: vetor de flags para evitar ciclos (não voltar a um aeroporto já visitado no caminho atual).
- `caminho[]`: guarda os índices dos aeroportos visitados no caminho atual.
- `profundidade`: posição atual no vetor `caminho`.

Funcionamento:
1. Marca o aeroporto atual como visitado e o registra no caminho.
2. Se chegou ao destino, imprime o caminho completo.
3. Caso contrário, percorre todos os vizinhos (voos saindo do atual) e chama `dfs` recursivamente para cada um não visitado.
4. **Desmarca o aeroporto ao retornar** (`visitado[atual] = 0`). Isso é essencial: permite que o mesmo aeroporto apareça em caminhos diferentes (ex: GRU pode ser escala em BSB→GRU→GIG e também em CNF→GRU→GIG).

Exemplo de execução para CNF → SSA:
```
dfs(CNF)
  → tenta GIG
      dfs(GIG)
        → tenta CNF (visitado, pula)
        → tenta GRU
            dfs(GRU)
              → tenta BSB
                  dfs(BSB)
                    → tenta SSA ✓  imprime: CNF → GIG → GRU → BSB → SSA
              → ...
  → tenta GRU (direto)
      ...
  → tenta SSA ✓  imprime: CNF → SSA
```

---

#### `listarTrajetos(GrafoAeroportos *g, const char *codigoOrigem, const char *codigoDestino)`

Valida os dois aeroportos, aloca os vetores `visitado` e `caminho` como arrays locais de tamanho `g->quantidade`, e dispara o `dfs`.

---

#### `listarAeroportos(GrafoAeroportos *g)`

Imprime o vetor de aeroportos em formato de tabela usando formatação de colunas com `%-6d`, `%-3s` etc. (alinhamento à esquerda com largura mínima).

---

## Módulo: `main.c`

### Funções Auxiliares

#### `limparBuffer(void)`

```c
while ((c = getchar()) != '\n' && c != EOF);
```

Descarta todos os caracteres que restaram no buffer de entrada até encontrar uma quebra de linha ou fim de arquivo. Necessária após `scanf`, que não consome o `'\n'` final, evitando que ele seja lido pela próxima operação.

---

#### `lerString(const char *prompt, char *destino, int tamanho)`

Usa `fgets` em vez de `scanf` para ler strings, pois `fgets` respeita o tamanho do buffer e lê espaços. O `strcspn` localiza o `'\n'` que `fgets` inclui na string e o substitui por `'\0'`:

```c
destino[strcspn(destino, "\n")] = '\0';
```

---

### Loop Principal do Menu

O loop `do { ... } while (opcao != 0)` repete o menu até o usuário escolher sair. A leitura da opção tem duas verificações:

```c
if (scanf("%d", &opcao) != 1) { ... }  // não conseguiu ler um inteiro
int prox = getchar();
if (prox != '\n' && prox != EOF) { ... } // tem conteúdo extra na linha (ex: "1 0")
```

A segunda verificação garante que entradas como `"1 0"` ou `"2abc"` sejam rejeitadas, mesmo que o `scanf` tenha lido o primeiro número com sucesso.

---

## Dados Iniciais (Hardcoded)

O `main` cadastra 5 aeroportos e 10 voos na inicialização para facilitar os testes. O grafo resultante, com seus voos, representa a tabela mostrada na seção do grafo acima.
