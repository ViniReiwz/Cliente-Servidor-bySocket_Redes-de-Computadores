#ifndef CLI_UTILS_H
#define CLI_UTILS_h

#include "gchat.h"

#define INP_INTERF "Você diz: "

// Definições de tipo =======================================================================================

// Estrutura auxiliar para enviar os argumentos do cliente à thread
typedef struct _cli_msg_args
{
    int cliSockFD;                  // Descritor do socket
    int* whisp_to;                  // Variável que guarda à quem está sussurando
    USER* user;                     // Usuário do cliente
    struct sockaddr_in* sockAddr;   // Endereço do scoket
}CLI_MSG_ARGS;

// ==========================================================================================================

// Funções ==================================================================================================

/**
 * Exibe as mensagens recebidas do servidor na tela
 *  params:
 *      MESSAGE* msg ->> Mensagem a ser exibida
 *      USER* user ->> Usuário do cliente
 */
void showMessage(MESSAGE* msg, USER* user);

/**
 * Resolve os comandos no lado do cliente
 *  params:
 *      MESSAGE* msg ->> Mensagem recebida do servidor
 *      USER* user ->> Usuário do cliente
 *      int* whisp_to ->> Ponteiro para o ID de quem deve sussurrar
 */
void cliResolveCommands(MESSAGE* msg, USER* user, int* whisp_to);

/**
 * Rotina para registrar usuário
 *  params:
 *      const int cliSockFD ->> Socket do cliente
 *  returns:
 *      USER* ->> Usuário registrado no servidor
 */
USER* registerUser(const int cliSockFD);


/**
 * Rotina de envio de mensagem do cliente
 *  params:
 *      USER* user ->> Usuário do cliente
 *      int cliSockFD ->> Socket do cliente
 *      const int* whisp_to ->> ID do usuário à sussurrar (-1 para broadcast)
 */
void cliSndMsg(USER* user, int cliSockFD, const int* whisp_to);

/**
 * Função chamada pela thread do cliente, que recebe constantemente mensagens do servidor, até que o cliente 
 * deseje sair do chat
 *  params:
 *      void* args ->> Convertido em CLI_MSG_ARGS* que contém os argumentos necessários para o recebimento 
 *                      de mensagens do cliente
 */
void* rcvMsgThread(void* args);

/**
 * Começa a receber as mensagens vindas do servidor, com uma thread separada do envio
 *  params:
 *      USER* user ->> Usuário do cliente
 *      int cliSockFD ->> Descritor do socket do cliente
 *      int* whisp_to ->> Variável auxiliar pra decidir para quem enviar as mensagens (broadcast ou usuário 
 *                          uńico)
 *      struct sockaddr_in* sockAddr ->> Endereço do socket
 */
void startReceiving(USER* user, int cliSockFD, int* whisp_to, struct sockaddr_in* sockAddr);

// ==========================================================================================================

#endif