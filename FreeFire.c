#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // Para usar o tipo bool

// --- Constantes ---
#define MAX_ITENS 10
#define MAX_NOME 50
#define MAX_TIPO 30

// --- Enumeração para Critérios de Ordenação ---
typedef enum {
    NOME,
    TIPO,
    PRIORIDADE
} CriterioOrdenacao;

// --- Estrutura para o Item de Sobrevivência ---
typedef struct {
    char nome[MAX_NOME];
    char tipo[MAX_TIPO];
    int quantidade;
    int prioridade; // Novo campo: 1 (baixa) a 5 (alta)
} Item;

// --- Variável Global de Estado (para controle de pré-requisitos) ---
// Rastreia por qual campo a lista está ordenada atualmente. 
// Usado principalmente para validar a Busca Binária (deve estar ordenada por NOME).
CriterioOrdenacao estado_ordenacao = NOME;
bool esta_ordenada = false;

// --- Funções Auxiliares ---

/**
 * @brief Limpa o buffer de entrada após o uso do scanf.
 */
void limpar_buffer_entrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Lê uma string de forma segura, descartando o newline.
 */
void ler_string_segura(char *buffer, int tamanho) {
    if (fgets(buffer, tamanho, stdin) != NULL) {
        // Remove o '\n' (se houver)
        buffer[strcspn(buffer, "\n")] = 0;
    }
}

/**
 * @brief Encontra o índice de um item pelo nome.
 * @return int O índice do item, ou -1 se não encontrado.
 */
int buscar_indice_por_nome(const Item mochila[], int total, const char *nome_alvo) {
    for (int i = 0; i < total; i++) {
        if (strcmp(mochila[i].nome, nome_alvo) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Exibe os detalhes de um único item.
 */
void exibir_item(const Item *item, int indice) {
    printf("\n--- 🔎 Item %d Encontrado ---\n", indice + 1);
    printf("  Nome:       %s\n", item->nome);
    printf("  Tipo:       %s\n", item->tipo);
    printf("  Quantidade: %d\n", item->quantidade);
    printf("  Prioridade: %d (1=Baixa, 5=Alta)\n", item->prioridade);
    printf("--------------------------------\n");
}

// --- Funções de Manipulação da Mochila ---

/**
 * @brief Adiciona um novo item à mochila.
 */
void adicionar_item(Item mochila[], int *total) {
    if (*total >= MAX_ITENS) {
        printf("\n❌ Mochila Cheia! Não é possível adicionar mais itens.\n");
        return;
    }

    Item novo_item;
    printf("\n--- Adicionar Novo Item ---\n");

    printf("Nome: ");
    ler_string_segura(novo_item.nome, MAX_NOME);

    printf("Tipo: ");
    ler_string_segura(novo_item.tipo, MAX_TIPO);

    printf("Quantidade: ");
    if (scanf("%d", &novo_item.quantidade) != 1 || novo_item.quantidade <= 0) {
        printf("⚠️ Quantidade inválida. Item não adicionado.\n");
        limpar_buffer_entrada();
        return;
    }
    limpar_buffer_entrada();

    printf("Prioridade (1 a 5, sendo 5 a mais alta): ");
    if (scanf("%d", &novo_item.prioridade) != 1 || novo_item.prioridade < 1 || novo_item.prioridade > 5) {
        printf("⚠️ Prioridade inválida. Item não adicionado.\n");
        limpar_buffer_entrada();
        return;
    }
    limpar_buffer_entrada();

    mochila[*total] = novo_item;
    (*total)++;
    esta_ordenada = false; // A adição de um novo item quebra qualquer ordenação anterior
    printf("\n✅ Item '%s' adicionado com sucesso.\n", novo_item.nome);
}

/**
 * @brief Remove um item da mochila pelo nome.
 */
void remover_item(Item mochila[], int *total) {
    if (*total == 0) {
        printf("\n❌ Mochila vazia. Nada para remover.\n");
        return;
    }

    char nome_alvo[MAX_NOME];
    printf("\n--- Remover Item ---\n");
    printf("Digite o nome do item a ser removido: ");
    ler_string_segura(nome_alvo, MAX_NOME);

    int indice = buscar_indice_por_nome(mochila, *total, nome_alvo);

    if (indice != -1) {
        // Move o último item para a posição do item removido
        for (int i = indice; i < *total - 1; i++) {
            mochila[i] = mochila[i + 1];
        }
        (*total)--;
        esta_ordenada = false; // A remoção altera a ordem
        printf("\n✅ Item '%s' removido com sucesso.\n", nome_alvo);
    } else {
        printf("\n❌ Item '%s' não encontrado na mochila.\n", nome_alvo);
    }
}

/**
 * @brief Lista todos os itens da mochila em formato de tabela.
 */
void listar_itens(const Item mochila[], int total) {
    if (total == 0) {
        printf("\n🪹 A mochila está vazia.\n");
        return;
    }

    printf("\n==================================================================\n");
    printf("                  🎒 ITENS DA MOCHILA (Total: %d) 🎒\n", total);
    printf("==================================================================\n");
    printf("| # | Nome                                | Tipo         | Qtd | Prio |\n");
    printf("|---|-------------------------------------|--------------|-----|------|\n");
    for (int i = 0; i < total; i++) {
        printf("| %d | %-35s | %-12s | %-3d | %-4d |\n",
               i + 1,
               mochila[i].nome,
               mochila[i].tipo,
               mochila[i].quantidade,
               mochila[i].prioridade);
    }
    printf("------------------------------------------------------------------\n");
    if (esta_ordenada) {
        const char *criterio_str;
        if (estado_ordenacao == NOME) criterio_str = "Nome";
        else if (estado_ordenacao == TIPO) criterio_str = "Tipo";
        else criterio_str = "Prioridade";
        printf("Estado: Ordenada por %s.\n", criterio_str);
    } else {
        printf("Estado: Desordenada.\n");
    }
}

// --- Funções de Ordenação (Insertion Sort) ---

/**
 * @brief Compara dois itens com base no critério escolhido.
 * @return int < 0 se item_a < item_b, 0 se item_a == item_b, > 0 se item_a > item_b.
 */
int comparar_itens(const Item *item_a, const Item *item_b, CriterioOrdenacao criterio, long long *comparacoes) {
    (*comparacoes)++;
    switch (criterio) {
        case NOME:
            return strcmp(item_a->nome, item_b->nome);
        case TIPO:
            return strcmp(item_a->tipo, item_b->tipo);
        case PRIORIDADE:
            // Ordenação decrescente por Prioridade (maior vem primeiro)
            return item_b->prioridade - item_a->prioridade;
    }
    return 0;
}

/**
 * @brief Ordena a mochila usando o algoritmo Insertion Sort.
 */
void ordenar_mochila(Item mochila[], int total, CriterioOrdenacao criterio) {
    if (total <= 1) {
        printf("\n⚠️ Mochila com 0 ou 1 item, já está ordenada.\n");
        esta_ordenada = true;
        estado_ordenacao = criterio;
        return;
    }

    long long comparacoes = 0;
    
    // Insertion Sort
    for (int i = 1; i < total; i++) {
        Item chave = mochila[i];
        int j = i - 1;

        // Move os elementos de mochila[0..i-1] que são maiores que a chave
        // para uma posição à frente de sua posição atual
        while (j >= 0 && comparar_itens(&mochila[j], &chave, criterio, &comparacoes) > 0) {
            mochila[j + 1] = mochila[j];
            j = j - 1;
        }
        mochila[j + 1] = chave;
    }

    esta_ordenada = true;
    estado_ordenacao = criterio;
    printf("\n✅ Mochila ordenada com sucesso. Critério: ");
    if (criterio == NOME) printf("Nome");
    else if (criterio == TIPO) printf("Tipo");
    else printf("Prioridade (Decrescente)");
    printf("\n🔬 Total de comparações na ordenação: %lld\n", comparacoes);
}

/**
 * @brief Exibe o menu de opções de ordenação e executa a escolha.
 */
void menu_ordenacao(Item mochila[], int total) {
    int escolha;
    printf("\n--- Escolha o Critério de Ordenação ---\n");
    printf("1. Nome\n");
    printf("2. Tipo\n");
    printf("3. Prioridade (Decrescente)\n");
    printf("Opção: ");

    if (scanf("%d", &escolha) != 1) {
        printf("\n❌ Entrada inválida.\n");
        limpar_buffer_entrada();
        return;
    }
    limpar_buffer_entrada();

    CriterioOrdenacao criterio;
    switch (escolha) {
        case 1:
            criterio = NOME;
            break;
        case 2:
            criterio = TIPO;
            break;
        case 3:
            criterio = PRIORIDADE;
            break;
        default:
            printf("\n❌ Opção de ordenação inválida.\n");
            return;
    }

    ordenar_mochila(mochila, total, criterio);
}

// --- Funções de Busca ---

/**
 * @brief Realiza busca binária pelo nome (requer que a lista esteja ordenada por NOME).
 * @return int O índice do item, ou -1 se não encontrado.
 */
int busca_binaria_por_nome(const Item mochila[], int total, const char *nome_alvo, long long *comparacoes) {
    if (!esta_ordenada || estado_ordenacao != NOME) {
        printf("\n❌ ERRO: A busca binária requer que a mochila esteja ordenada por NOME.\n");
        printf("Por favor, ordene a mochila por NOME (Opção 3 -> 1) antes de buscar.\n");
        return -1;
    }
    
    int esquerda = 0;
    int direita = total - 1;
    int resultado_comparacao;

    while (esquerda <= direita) {
        int meio = esquerda + (direita - esquerda) / 2;
        
        (*comparacoes)++; // Contagem de comparação principal
        resultado_comparacao = strcmp(mochila[meio].nome, nome_alvo);

        if (resultado_comparacao == 0) {
            return meio; // Encontrado
        } else if (resultado_comparacao < 0) {
            esquerda = meio + 1; // Nome está à direita
        } else {
            direita = meio - 1; // Nome está à esquerda
        }
    }
    
    return -1; // Não encontrado
}

/**
 * @brief Executa a busca binária interativa pelo nome.
 */
void realizar_busca_binaria(const Item mochila[], int total) {
    if (total == 0) {
        printf("\n❌ Mochila vazia. Não é possível buscar.\n");
        return;
    }
    
    char nome_alvo[MAX_NOME];
    printf("\n--- Busca Binária por Nome ---\n");
    printf("Digite o nome do item que deseja buscar: ");
    ler_string_segura(nome_alvo, MAX_NOME);

    long long comparacoes = 0;
    int indice = busca_binaria_por_nome(mochila, total, nome_alvo, &comparacoes);

    if (indice != -1) {
        exibir_item(&mochila[indice], indice);
        printf("🔬 Total de comparações na busca: %lld\n", comparacoes);
    } else if (esta_ordenada && estado_ordenacao == NOME) {
        printf("\n❌ Item '%s' não encontrado (comparacoes: %lld).\n", nome_alvo, comparacoes);
    }
    // Se a lista não estiver ordenada por NOME, a função já exibiu uma mensagem de erro.
}


// --- Função Principal ---

int main() {
    Item mochila[MAX_ITENS];
    int total_itens = 0;
    int opcao;

    printf("--- 🏝️ Desafio Código da Ilha (Nível Mestre) – Mochila Free Fire 🔫 ---\n");

    do {
        printf("\n======================== MENU PRINCIPAL ========================\n");
        printf("1. Adicionar item (Nome, Tipo, Qtd, Prioridade)\n");
        printf("2. Remover item por nome\n");
        printf("3. Listar itens (Exibir estado)\n");
        printf("4. Ordenar mochila (Nome, Tipo, Prioridade)\n");
        printf("5. Buscar item por nome (Busca Binária - Requer ordenação por Nome)\n");
        printf("0. Sair e escapar da ilha\n");
        printf("Escolha uma opção: ");

        if (scanf("%d", &opcao) != 1) {
            printf("\n❌ Entrada inválida. Tente novamente.\n");
            limpar_buffer_entrada();
            opcao = -1; // Garante que o loop continue
            continue;
        }
        limpar_buffer_entrada();

        switch (opcao) {
            case 1:
                adicionar_item(mochila, &total_itens);
                break;
            case 2:
                remover_item(mochila, &total_itens);
                break;
            case 3:
                listar_itens(mochila, total_itens);
                break;
            case 4:
                menu_ordenacao(mochila, total_itens);
                break;
            case 5:
                realizar_busca_binaria(mochila, total_itens);
                break;
            case 0:
                printf("\n👋 Sobrevivente, você escapou da ilha! Fim do programa.\n");
                break;
            default:
                printf("\n❌ Opção inválida. Digite um número de 0 a 5.\n");
                break;
        }
    } while (opcao != 0);

    return 0;
}