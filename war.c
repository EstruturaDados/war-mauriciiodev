#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// ============================================================================
// --- Constantes Globais ---
// ============================================================================
#define MAX_NOME_TERRITORIO 30
#define MAX_COR_EXERCITO 10
#define NUM_TERRITORIOS 20 // Mapa simplificado com 20 territórios
#define NUM_MISSOES 4      // Número de missões disponíveis
#define MAX_ROLO_DADOS 3   // Máximo de dados que podem ser rolados no ataque/defesa

// ============================================================================
// --- Estrutura de Dados ---
// ============================================================================
// Estrutura para representar um único território no mapa
typedef struct {
    char nome[MAX_NOME_TERRITORIO];
    char cor_exercito[MAX_COR_EXERCITO]; // Cor do exército que domina o território
    int tropas;                          // Número de tropas alocadas
} Territorio;

// ============================================================================
// --- Protótipos das Funções ---
// ============================================================================

// Funções de setup e gerenciamento de memória
Territorio* alocarMapa();
void inicializarTerritorios(Territorio* mapa);
void definirDonoInicial(Territorio* mapa, const char* cor_jogador);
void liberarMemoria(Territorio* mapa);

// Funções de interface com o usuário
void exibirMenuPrincipal();
void exibirMapa(const Territorio* mapa, int num_territorios);
void exibirMissao(int id_missao);
void limparBufferEntrada();
void pausarExecucao();

// Funções de lógica principal do jogo
void faseDeAtaque(Territorio* mapa, const char* cor_jogador);
void simularAtaque(Territorio* atacante, Territorio* defensor);
int sortearMissao();
int verificarVitoria(const Territorio* mapa, int id_missao, const char* cor_jogador);

// Função utilitária
int rolarDado();

// ============================================================================
// --- Função Principal (main) ---
// ============================================================================
int main() {
    // 1. Configuração Inicial (Setup)
    setlocale(LC_ALL, "pt_BR.UTF-8"); // Define o locale para português
    srand(time(NULL));                 // Inicializa a semente para números aleatórios

    Territorio* mapa = alocarMapa();
    if (mapa == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para o mapa.\n");
        return 1;
    }

    // Cor de Exemplo: O jogador será o exército "Azul"
    const char cor_jogador[MAX_COR_EXERCITO] = "Azul";

    // Inicializa o mapa com nomes e tropas/donos iniciais
    inicializarTerritorios(mapa);
    definirDonoInicial(mapa, cor_jogador);

    int id_missao = sortearMissao();
    int escolha_jogador = -1;
    int jogo_ativo = 1;
    int vitoria = 0;

    printf("🗺️ PROJETO WAR ESTRUTURADO - INÍCIO DO JOGO 🗺️\n");
    printf("Você está no comando do exército: **%s**\n", cor_jogador);

    // 2. Laço Principal do Jogo (Game Loop)
    do {
        printf("\n--- RODADA ATUAL ---\n");
        exibirMapa(mapa, NUM_TERRITORIOS);
        exibirMissao(id_missao);
        exibirMenuPrincipal();

        printf("Sua escolha: ");
        if (scanf("%d", &escolha_jogador) != 1) {
            printf("Entrada inválida. Tente novamente.\n");
            limparBufferEntrada();
            escolha_jogador = -1; // Força uma nova iteração
            pausarExecucao();
            continue;
        }
        limparBufferEntrada();

        switch (escolha_jogador) {
            case 1: // Iniciar Ataque
                faseDeAtaque(mapa, cor_jogador);
                break;
            case 2: // Verificar Vitória
                vitoria = verificarVitoria(mapa, id_missao, cor_jogador);
                if (vitoria) {
                    printf("\n🎉 PARABÉNS! VOCÊ CUMPRIU SUA MISSÃO! 🎉\n");
                    jogo_ativo = 0;
                } else {
                    printf("\n🔍 Missão não cumprida. Continue lutando!\n");
                }
                break;
            case 0: // Sair
                printf("\n🔚 O jogo foi encerrado. Até a próxima!\n");
                jogo_ativo = 0;
                break;
            default:
                printf("\n❌ Opção inválida. Escolha 1, 2 ou 0.\n");
                break;
        }

        if (jogo_ativo) {
            pausarExecucao();
        }

    } while (jogo_ativo);

    // 3. Limpeza
    liberarMemoria(mapa);
    return 0;
}

// ============================================================================
// --- Implementação das Funções de Setup e Gerenciamento de Memória ---
// ============================================================================

/**
 * @brief Aloca dinamicamente a memória para o vetor de territórios.
 * @return Ponteiro para a memória alocada ou NULL em caso de falha.
 */
Territorio* alocarMapa() {
    // Aloca memória para NUM_TERRITORIOS estruturas Territorio e inicializa com zero.
    Territorio* mapa = (Territorio*)calloc(NUM_TERRITORIOS, sizeof(Territorio));
    return mapa;
}

/**
 * @brief Preenche os dados iniciais de cada território no mapa.
 * Esta função MODIFICA o mapa (passagem por referência).
 * @param mapa Ponteiro para o primeiro elemento do vetor de Territorios.
 */
void inicializarTerritorios(Territorio* mapa) {
    // Lista de nomes de territórios (simplificada)
    const char* nomes[] = {
        "Alaska", "Groelandia", "Ottawa", "NovaYork", "California",
        "Mexico", "Brasil", "Argentina", "Inglaterra", "Franca",
        "Egito", "AfricaSul", "Russia", "China", "India",
        "Japao", "Australia", "Indonesia", "Siberia", "Aral"
    };

    // Cores Iniciais dos Exércitos (para simulação de donos)
    const char* cores_iniciais[] = {
        "Azul", "Vermelho", "Amarelo", "Verde"
    };

    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        // Copia o nome do território
        strncpy(mapa[i].nome, nomes[i], MAX_NOME_TERRITORIO - 1);
        mapa[i].nome[MAX_NOME_TERRITORIO - 1] = '\0'; // Garantir terminação nula
        
        // Atribui cor inicial (rotativamente)
        strncpy(mapa[i].cor_exercito, cores_iniciais[i % 4], MAX_COR_EXERCITO - 1);
        mapa[i].cor_exercito[MAX_COR_EXERCITO - 1] = '\0';
        
        // Distribui tropas iniciais (entre 2 e 5)
        mapa[i].tropas = 2 + (rand() % 4);
    }
}

/**
 * @brief Garante que o jogador (cor_jogador) tenha alguns territórios no início.
 * @param mapa Ponteiro para o vetor de Territorios.
 * @param cor_jogador Cor do exército do jogador.
 */
void definirDonoInicial(Territorio* mapa, const char* cor_jogador) {
    // Garante que o jogador comece com pelo menos 5 territórios.
    int i = 0;
    int count = 0;
    while (count < 5 && i < NUM_TERRITORIOS) {
        if (strcmp(mapa[i].cor_exercito, cor_jogador) != 0) {
             strncpy(mapa[i].cor_exercito, cor_jogador, MAX_COR_EXERCITO);
             mapa[i].tropas = 3 + (rand() % 3); // Reforça as tropas iniciais do jogador
             count++;
        }
        i++;
    }
}


/**
 * @brief Libera a memória alocada para o mapa.
 * @param mapa Ponteiro para o primeiro elemento do vetor de Territorios a ser liberado.
 */
void liberarMemoria(Territorio* mapa) {
    if (mapa != NULL) {
        free(mapa);
        printf("\nMemória do mapa liberada com sucesso.\n");
    }
}

// ============================================================================
// --- Implementação das Funções de Interface com o Usuário ---
// ============================================================================

/**
 * @brief Limpa o buffer de entrada para evitar erros de leitura.
 */
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Pausa a execução do programa e espera o usuário pressionar Enter.
 */
void pausarExecucao() {
    printf("\nPressione ENTER para continuar...\n");
    getchar();
}

/**
 * @brief Exibe o menu de ações do jogo.
 */
void exibirMenuPrincipal() {
    printf("\n--- MENU DE AÇÕES ---\n");
    printf("1. Iniciar Fase de Ataque\n");
    printf("2. Verificar Condição de Vitória (Missão)\n");
    printf("0. Sair do Jogo\n");
    printf("---------------------\n");
}

/**
 * @brief Mostra o estado atual de todos os territórios.
 * Usa 'const' para garantir que a função apenas LEIA os dados.
 * @param mapa Ponteiro constante para o vetor de Territorios.
 * @param num_territorios Número de territórios a serem exibidos.
 */
void exibirMapa(const Territorio* mapa, int num_territorios) {
    printf("\n====================================================================\n");
    printf("| %-3s | %-28s | %-9s | %-6s |\n", "ID", "NOME DO TERRITÓRIO", "DONO", "TROPAS");
    printf("====================================================================\n");

    for (int i = 0; i < num_territorios; i++) {
        printf("| %-3d | %-28s | %-9s | %-6d |\n",
               i,
               mapa[i].nome,
               mapa[i].cor_exercito,
               mapa[i].tropas);
    }
    printf("====================================================================\n");
}

/**
 * @brief Exibe a descrição da missão secreta do jogador.
 * @param id_missao ID da missão sorteada.
 */
void exibirMissao(int id_missao) {
    printf("\n🛡️ SUA MISSÃO SECRETA: ");
    switch (id_missao) {
        case 1:
            printf("Conquistar 18 territórios (exclui o continente da Oceania).\n");
            break;
        case 2:
            printf("Destruir o exército **VERMELHO**. Se a cor não existir, sua missão muda para conquistar 24 territórios.\n");
            break;
        case 3:
            printf("Conquistar os continentes da América do Norte e Europa.\n");
            break;
        case 4:
            printf("Destruir o exército **AMARELO**. Se a cor não existir, sua missão muda para conquistar 24 territórios.\n");
            break;
        default:
            printf("Missão desconhecida (ID: %d). Sua missão é conquistar 30 territórios.\n", id_missao);
            break;
    }
    printf("\n");
}

// ============================================================================
// --- Implementação das Funções de Lógica Principal do Jogo ---
// ============================================================================

/**
 * @brief Rola um dado de 6 faces.
 * @return Um valor aleatório entre 1 e 6.
 */
int rolarDado() {
    // RAND_MAX é geralmente 32767. (rand() % 6) + 1 garante um resultado entre 1 e 6.
    return (rand() % 6) + 1;
}

/**
 * @brief Simula o ataque entre dois territórios.
 * Esta função MODIFICA as tropas dos territórios (passagem por referência).
 * @param atacante Ponteiro para o território que está atacando.
 * @param defensor Ponteiro para o território que está se defendendo.
 */
void simularAtaque(Territorio* atacante, Territorio* defensor) {
    // O atacante rola um dado, o defensor rola um dado.
    int dado_ataque = rolarDado();
    int dado_defesa = rolarDado();

    printf("\n--- RESULTADO DO COMBATE ---\n");
    printf("  %s (Atacante): Rolou %d\n", atacante->cor_exercito, dado_ataque);
    printf("  %s (Defensor): Rolou %d\n", defensor->cor_exercito, dado_defesa);

    if (dado_ataque > dado_defesa) {
        printf("  👉 ATAQUE VENCE! Defensor perde 1 tropa.\n");
        defensor->tropas--;
    } else { // Empate ou Defesa Vence
        printf("  👉 DEFESA VENCE/EMPATE! Atacante perde 1 tropa.\n");
        atacante->tropas--;
    }
    printf("----------------------------\n");

    // Verifica se o território foi conquistado
    if (defensor->tropas <= 0) {
        printf("🎉 CONQUISTA! O território **%s** foi conquistado por %s! 🎉\n",
               defensor->nome, atacante->cor_exercito);

        // Mudar o dono e mover pelo menos 1 tropa
        strncpy(defensor->cor_exercito, atacante->cor_exercito, MAX_COR_EXERCITO);
        
        // Mover 1 tropa (mínimo) do atacante para o defensor
        atacante->tropas--;
        defensor->tropas = 1;

        printf("  >> 1 tropa movida para o território conquistado. Tropas restantes no atacante: %d\n", atacante->tropas);
    }
}

/**
 * @brief Gerencia a fase de ataque e valida as entradas.
 * @param mapa Ponteiro para o vetor de Territorios.
 * @param cor_jogador Cor do exército do jogador (passagem por valor/referência constante).
 */
void faseDeAtaque(Territorio* mapa, const char* cor_jogador) {
    int id_origem, id_destino;

    printf("\n--- FASE DE ATAQUE ---\n");
    printf("Digite o ID do território de **ORIGEM** (seu): ");
    if (scanf("%d", &id_origem) != 1 || id_origem < 0 || id_origem >= NUM_TERRITORIOS) {
        printf("ID de origem inválido.\n");
        limparBufferEntrada();
        return;
    }
    limparBufferEntrada(); // Limpa o buffer após a leitura do ID

    // Validação 1: O território de origem pertence ao jogador?
    if (strcmp(mapa[id_origem].cor_exercito, cor_jogador) != 0) {
        printf("❌ Erro: O território de ID %d (%s) não pertence ao seu exército (%s).\n", 
               id_origem, mapa[id_origem].nome, cor_jogador);
        return;
    }

    // Validação 2: O território de origem tem tropas suficientes para atacar (> 1)?
    if (mapa[id_origem].tropas <= 1) {
        printf("❌ Erro: O território de ID %d (%s) precisa de pelo menos 2 tropas para atacar.\n", 
               id_origem, mapa[id_origem].nome);
        return;
    }

    printf("Digite o ID do território de **DESTINO** (inimigo): ");
    if (scanf("%d", &id_destino) != 1 || id_destino < 0 || id_destino >= NUM_TERRITORIOS) {
        printf("ID de destino inválido.\n");
        limparBufferEntrada();
        return;
    }
    limparBufferEntrada(); // Limpa o buffer após a leitura do ID

    // Validação 3: O território de destino pertence a um inimigo?
    if (strcmp(mapa[id_destino].cor_exercito, cor_jogador) == 0) {
        printf("❌ Erro: O território de ID %d (%s) já pertence ao seu exército.\n", 
               id_destino, mapa[id_destino].nome);
        return;
    }
    
    // Validação 4: Não pode atacar a si mesmo (embora a validação 3 já trate isso)
    if (id_origem == id_destino) {
        printf("❌ Erro: O território de destino não pode ser o mesmo que o de origem.\n");
        return;
    }


    // Chamada à função principal da lógica de ataque
    simularAtaque(&mapa[id_origem], &mapa[id_destino]);
}

/**
 * @brief Sorteia um ID de missão aleatório para o jogador.
 * @return O ID da missão sorteada (entre 1 e NUM_MISSOES).
 */
int sortearMissao() {
    return (rand() % NUM_MISSOES) + 1;
}

/**
 * @brief Verifica se o jogador cumpriu os requisitos de sua missão.
 * @param mapa Ponteiro constante para o vetor de Territorios.
 * @param id_missao ID da missão a ser verificada.
 * @param cor_jogador Cor do exército do jogador.
 * @return 1 (verdadeiro) se a missão foi cumprida, e 0 (falso) caso contrário.
 */
int verificarVitoria(const Territorio* mapa, int id_missao, const char* cor_jogador) {
    int territorios_conquistados = 0;
    int cor_alvo_destruida = 1; // Assume que a cor alvo foi destruída

    // Conta quantos territórios o jogador possui
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        if (strcmp(mapa[i].cor_exercito, cor_jogador) == 0) {
            territorios_conquistados++;
        }
    }

    switch (id_missao) {
        case 1: // Conquistar 18 territórios (exclui Oceania - IDs 16, 17, 18)
            // Lógica simplificada: apenas conta o total de territórios.
            // A lógica de exclusão de continentes é mais complexa e omitida aqui.
            return (territorios_conquistados >= 18);

        case 2: { // Destruir o exército VERMELHO ou conquistar 24 (se Vermelho não existir)
            const char* cor_alvo = "Vermelho";
            
            // Verifica se a cor alvo ainda existe no mapa
            for (int i = 0; i < NUM_TERRITORIOS; i++) {
                if (strcmp(mapa[i].cor_exercito, cor_alvo) == 0) {
                    cor_alvo_destruida = 0; // Ainda existe pelo menos 1 território Vermelho
                    break;
                }
            }
            
            // Verifica se o jogador possui a cor alvo
            int jogador_possui_alvo = 0;
            for (int i = 0; i < NUM_TERRITORIOS; i++) {
                if (strcmp(mapa[i].cor_exercito, cor_jogador) == 0 && strcmp(mapa[i].cor_exercito, cor_alvo) == 0) {
                    jogador_possui_alvo = 1;
                    break;
                }
            }

            // Se o jogador é o Vermelho, a missão muda automaticamente
            if (strcmp(cor_jogador, cor_alvo) == 0) {
                return (territorios_conquistados >= 24); // Missão alternativa
            }

            // Missão de destruição: O jogador destrói a cor alvo?
            if (cor_alvo_destruida) {
                return 1;
            }
            
            // Se a cor alvo não existe no início, a missão alternativa entra em vigor
            // Nota: Uma implementação mais robusta checaria se o jogador DESTRUIU o exército.
            // Aqui, apenas verificamos se a cor alvo existe.
            return (territorios_conquistados >= 24); 
        }

        case 3: // Conquistar os continentes da América do Norte e Europa.
            // Lógica complexa de verificação de continente omitida para manter o foco na modularização.
            // Simplificado para: conquistar um número grande de territórios.
            return (territorios_conquistados >= 15);
            
        case 4: { // Destruir o exército AMARELO ou conquistar 24 (se Amarelo não existir)
            const char* cor_alvo = "Amarelo";
            cor_alvo_destruida = 1;
            
            // Verifica se a cor alvo ainda existe no mapa
            for (int i = 0; i < NUM_TERRITORIOS; i++) {
                if (strcmp(mapa[i].cor_exercito, cor_alvo) == 0) {
                    cor_alvo_destruida = 0;
                    break;
                }
            }
            
            // Se o jogador é o Amarelo, a missão muda automaticamente
            if (strcmp(cor_jogador, cor_alvo) == 0) {
                return (territorios_conquistados >= 24); // Missão alternativa
            }
            
            // Missão de destruição ou alternativa
            if (cor_alvo_destruida) {
                return 1;
            }
            return (territorios_conquistados >= 24);
        }

        default:
            return (territorios_conquistados >= 30); // Missão genérica de dominação
    }
}