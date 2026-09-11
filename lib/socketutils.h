#ifndef SOCKET_UTILS
#define SOCKET_UTILS

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CNCT_PORT 8080  // Porta de conexão (cliente e servidor)

#define SRV_SIDE 1      // Constantes auxiliares para definir se o socket é do lado do servidor
#define CLI_SIDE 2      // ou do cliente

/*
    Cria um socket para TCP, o nomeia (faz o bind) e retorna seu descritor.
    O socket se conecta à `INADDR_ANY` pela porta definida em `CNCT_PORT`

    params:
        const int side ->> Lado da conexão (`SRV_SIDE` ou `CLI_SIDE`)

    returns:
        int socketFD ->> Descritor do socket já nomeado
*/
int createTCPSocket(const int side);

#endif