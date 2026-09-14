#ifndef SOCKET_UTILS
#define SOCKET_UTILS

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CNCT_PORT 8080  // Porta de conexão (cliente e servidor)

#define SRV_SIDE 1      // Constantes auxiliares para definir se o socket é do lado do servidor
#define CLI_SIDE 2      // ou do cliente

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

#endif