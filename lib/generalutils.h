#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H

// Funções ==================================================================================================

/**
 * Recupera o argumento de um comando do chat
 *  params:
 *      char* command ->> Comando inteiro "/'X' 'arg'"
 *  returns:
 *      char* ->> Argumento do comando (nome ou id) | NULL caso não tenha. ("/q" ou "/r")
 */
char* getCommandArg(char* command);

/*
    Função auxiliar para pegar o ID do servidor da entrada que o usuário digitar

    returns:
        char* srv_ip ->> Ponteiro alocado dinâmicamente que contém a string do IP
*/
char* inputServerIP();

/**
 * Exibe os dados do usuário
 *  params:
 *      USER* user ->> Usuário a ter dados exibidos
 */
void printUserData(USER* user);

/**
 * Exibe toda a lista de usuários
 *  params:
 *      USER_LIST* list ->> Lista a ser exibida
 */
void printListData(USER_LIST* list);

/**
 * Exibe uma mensagem, com todas as informações relevantes
 *  params:
 *      MESSAGE* message ->> Mensagem a ser exibida
 */
void printMsg(MESSAGE* message);

/**
 * Pega um input da tela
 *  params:
 *      const int len ->> Tamanho do input
 *  returns:
 *      char* ->> Texto recebido
 */
char* getStringInput(const int len);

// ==========================================================================================================

#endif