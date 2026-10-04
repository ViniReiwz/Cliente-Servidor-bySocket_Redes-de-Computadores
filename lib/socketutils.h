#ifndef SOCKET_UTILS
#define SOCKET_UTILS

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "userutils.h"

// Definições de tipos ======================================================================================

// Estrutura que define as mensagens trocadas entre cliente e servidor
typedef struct _messsage 
{
    USER sender;                // Usuário que enviou a mensagem
    char message[MSG_SIZE];     // Mensagem digitada pelo usuário
    int to_id;                  // Id do sussuro (-1 para falar para todo o chat)
    int comm;                   // Campo auxiliar que indica o comando a ser realizado pelo cliente
}MESSAGE;

// Estrutura que define informações importantes para um socket de conexão aceita
typedef struct _accepted_socket
{
    int sockFD;                     // Descritor do socket
    struct sockaddr_in* sockAddr;   // Endereço do socket
    int wasAccpeted;                // Campo que indica se foi aceito ou não
    int error;                      // Erro de conexão (caso tenha)
}ACCEPTED_SOCKET;

// ==========================================================================================================

// Funções ==================================================================================================

/*
    Cria um socket para TCP, o nomeia, faz o bind (no lado do servidor) e retorna seu descritor.
    O socket se conecta à `INADDR_ANY` pela porta definida em `CNCT_PORT`, no lado do servidor

    params:
        const int side ->> Lado da conexão (`SRV_SIDE` ou `CLI_SIDE`)
        const int socketFD ->> Descritor do socket

    returns:
        strcut sockaddr_in sockAddr ->> Estrutura com as informações do socket |
        NULL caso aja algum erro na criação do mesmo
*/
struct sockaddr_in* createSocketAddrIPV4(const int side, const int socketFD);

/**
 * Constrói a mensagem a ser enviada
 *  params:
 *      USER* user ->> Usuário que envia a mensagem
 *      const char* text ->> Mensagem de texto própriamente dita
 *      const int whisp_to ->> ID do usuário para qual sussurra (-1 para enviar à todos)
 *  returns:
 *      MESSAGE* ->> Ponteiro para estrutura MESSAGE que contém a mensagem e o usuário remetente
 */
MESSAGE* buildMessage(USER* user, char* text, const int whisp_to);

/**
 * Aceita uma conexão no servidor
 *  params:
 *      int srvSockFD ->> Descritor do socket do servidor
 *  returns:
 *      ACCEPTERD_SOCKET* ->> Estrutura com todas as informações relevantes do socket aceito.
 */
ACCEPTED_SOCKET* accpetIncomingConnection(int srvSockFD);

/**
 * Libera a memória de um socket aceito e o fecha
 *  params:
 *      ACCCEPTED_SOCKET* acpt_sock ->> Socket a ser liberado
 */
void destroyAcptSock(ACCEPTED_SOCKET* acpt_sock);

/**
 * Recebe toda a mensagem
 *  params:
 *      int sockFD ->> Descritor do socket que envia a mensagem
 *      void* buffer ->> Buffer onde a mensagem deve ser armazenada
 *      size_t size ->> Tamanho da mensagem
 *  returns:
 *      size_t ->> Total de bytes recebidos
 */
size_t recvAll(int sockFD, void* buffer, size_t size);

/**
 * Recebe toda a mensagem
 *  params:
 *      int sockFD ->> Descritor do socket que envia a mensagem
 *      void* buffer ->> Buffer onde a mensagem deve ser armazenada
 *      size_t size ->> Tamanho da mensagem
 *  returns:
 *      size_t ->> Total de bytes recebidos
 */
size_t sendAll(int sockFD, void* buffer, size_t size);

// ==========================================================================================================

#endif