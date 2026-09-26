#ifndef SOCKET_UTILS
#define SOCKET_UTILS

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "userutils.h"

#define CNCT_PORT 8080  // Porta de conexão (cliente e servidor)

#define SRV_SIDE 1      // Constantes auxiliares para definir se o socket é do lado do servidor
#define CLI_SIDE 2      // ou do cliente

#define MSG_SIZE 1024
#define NAME_SIZE 50

typedef struct _messsage 
{
    USER sender;                // Usuário que enviou a mensagem
    char message[MSG_SIZE];     // Mensagem digitada pelo usuário
    int to_id;                  // Id do sussuro (-1 para falar para todo o chat)
}MESSAGE;

typedef struct _accepted_socket
{
    int sockFD;
    struct sockaddr_in* sockAddr;
    int wasAccpeted;
    int error;
}ACCEPTED_SOCKET;

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

ACCEPTED_SOCKET* accpetIncomingConnection(int srvSockFD);

void destroyAcptSock(ACCEPTED_SOCKET* acpt_sock);
#endif