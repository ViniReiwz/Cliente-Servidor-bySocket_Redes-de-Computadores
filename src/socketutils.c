#include "gchat.h"
#include "socketutils.h"

/*
    Função auxiliar para pegar o ID do servidor da entrada que o usuário digitar

    returns:
        char* srv_ip ->> Ponteiro alocado dinâmicamente que contém a string do IP
*/
char* inputServerIP()
{
    int valid = 0;
    
    char* srv_ip = (char*)calloc(INET_ADDRSTRLEN,sizeof(char));

    while(!valid)
    {
        fgets(srv_ip, INET_ADDRSTRLEN, stdin);
        srv_ip[strcspn(srv_ip,"\n")] = '\0';

        if(DEBUG){printf("IP registrado: \n\n%s\n\n", srv_ip);}

        // Verifica se está no formato mínimo "XXX.X.X.X"
        if(strlen(srv_ip) < 9){ printf("Escreva um endereço válido !\n"); }
        else { valid = 1; }
    }

    return srv_ip;
}

/*
    Cria um socket para TCP, o nomeia (faz o bind) e retorna seu descritor.
    O socket se conecta à `INADDR_ANY` pela porta definida em `CNCT_PORT`, no lado do servidor

    params:
        const int side ->> Lado da conexão (`SRV_SIDE` ou `CLI_SIDE`)
        int socketFD ->> Descritor do socket

    returns:
        strcut sockaddr_in* sockAddr ->> Estrutura com as informações do socket |
        NULL caso aja algum erro na criação do mesmo
*/
struct sockaddr_in* createSocketAddrIPV4(const int side, const int socketFD)
{

    // "Nomeia" o socket (o atrela à um endereço)
    /*
        Analisemos a struct sockadddr_in =>
            .sin_addr.s_addr ->> Endereço da máquina a se conectar (no caso, INADDR_ANY especifica que pode se conectar à qualquer IP desta máquina local seja localhost, o IP na rede local ou o IP externo)
            .sin_family ->> Especifica a família de protocolos (mesma do Socket criado anteriormente)
            .sin_port ->> Porta à qual o socket deve se conectar (mesma que o servidor)
    */
    struct sockaddr_in* sockAddr = (struct sockaddr_in*)calloc(1, sizeof(struct sockaddr_in));

    sockAddr->sin_family = AF_INET;
    sockAddr->sin_port = htons(CNCT_PORT);
    
    switch (side)
    {
        case SRV_SIDE:  // DO lado do servidor, aloca o endereço e faz o bind
        {
            sockAddr->sin_addr.s_addr = htonl(INADDR_ANY);

            int bindRes = bind(socketFD, (struct sockaddr *)sockAddr, sizeof(struct sockaddr_in));
            if(bindRes == -1)
            {
                perror("bind");
                printf("Erro ao fazer o bind no socket do servidor !\n");
                return NULL;
            }

            break;
        }
        
        case CLI_SIDE:  // Do lado do cliente, instância o socket indicando à qual server se conectar
        {
            printf("Digite o IP do servidor: ");
            char* srv_ip = inputServerIP();
            int res = inet_pton(AF_INET, srv_ip, &sockAddr->sin_addr.s_addr);
            free(srv_ip);
            if(res <= 0)
            {
                printf("Erro ao instanciar a socket para o cliente (endereço de IP inválido).\n");
                return NULL;
            }

            break;
        }

        default:
            {
                printf("Especifique o lado da conexão!!\n");
                return NULL;
            }
            
    }    

    return sockAddr;
}

/**
 * Aceita uma conexão no servidor
 *  params:
 *      int srvSockFD ->> Descritor do socket do servidor
 *  returns:
 *      ACCEPTERD_SOCKET* ->> Estrutura com todas as informações relevantes do socket aceito.
 */
ACCEPTED_SOCKET* accpetIncomingConnection(int srvSockFD)
{
    struct sockaddr_in* cliSockAddr = (struct sockaddr_in*)calloc(1,sizeof(struct sockaddr_in));
    int addrSize = sizeof(struct sockaddr);

    // Aceita a conexão do cliente
    int cliSockFD = accept(srvSockFD, (struct sockaddr*)cliSockAddr, (socklen_t*)&addrSize);

    // Monta a estrutura de  um socket aceito com todas as informações necessárias
    ACCEPTED_SOCKET* acpt_sock = (ACCEPTED_SOCKET*)calloc(1,sizeof(ACCEPTED_SOCKET));
    acpt_sock->sockFD = cliSockFD;
    acpt_sock->sockAddr = cliSockAddr;
    acpt_sock->wasAccpeted = cliSockFD > 0;
    if(!acpt_sock->wasAccpeted){ acpt_sock->error = cliSockFD; }

    return acpt_sock;
}

/**
 * Libera a memória de um socket aceito e o fecha
 *  params:
 *      ACCCEPTED_SOCKET* acpt_sock ->> Socket a ser liberado
 */
void destroyAcptSock(ACCEPTED_SOCKET* acpt_sock)
{
    close(acpt_sock->sockFD);
    free(acpt_sock->sockAddr);
    free(acpt_sock);
}

/**
 * Constrói a mensagem a ser enviada
 *  params:
 *      USER* user ->> Usuário que envia a mensagem
 *      const char* text ->> Mensagem de texto própriamente dita
 *      const int whisp_to ->> ID do usuário para qual sussurra (-1 para enviar à todos)
 *  returns:
 *      MESSAGE* ->> Ponteiro para estrutura MESSAGE que contém a mensagem e o usuário remetente
 */
MESSAGE* buildMessage(USER* user, char* text, const int whisp_to)
{
    MESSAGE* message = (MESSAGE*)calloc(1,sizeof(MESSAGE));

    message->sender.id = user->id;
    strcpy(message->sender.name, user->name);
    message->sender.sockFD = user->sockFD;
    strcpy(message->message, text);
    message->to_id = whisp_to;
    message->comm = NONE_COMM;

    return message;
}

/**
 * Recebe toda a mensagem
 *  params:
 *      int sockFD ->> Descritor do socket que envia a mensagem
 *      void* buffer ->> Buffer onde a mensagem deve ser armazenada
 *      size_t size ->> Tamanho da mensagem
 *  returns:
 *      size_t ->> Total de bytes recebidos
 */
size_t recvAll(int sockFD, void* buffer, size_t size)
{
    size_t total = 0;
    while(total < size)
    {
        ssize_t bytes = recv(sockFD, (char*)buffer + total, size - total, 0);
        if(bytes == 0){ return 0; }
        if (bytes < 0){ perror("recv"); return -1; }

        total += bytes;
    }

    return total;
}

/**
 * Recebe toda a mensagem
 *  params:
 *      int sockFD ->> Descritor do socket que envia a mensagem
 *      void* buffer ->> Buffer onde a mensagem deve ser armazenada
 *      size_t size ->> Tamanho da mensagem
 *  returns:
 *      size_t ->> Total de bytes recebidos
 */
size_t sendAll(int sockFD, void* buffer, size_t size)
{
    size_t total = 0;
    while(total < size)
    {
        ssize_t bytes = send(sockFD, (char*)buffer + total, size - total, 0);
        if(DEBUG) { printf("Bytes enviados nesse send\n: %li", bytes); }
        if (bytes <= 0){ perror("send"); return -1; }

        total += bytes;
    }

    return total;
}
