#include "gchat.h"
#include "socketutils.h"

/*
    Função auxiliar para pegar o ID do servidor da entrada que o usuário digitar

    returns:
        char* srv_ip ->> Ponteiro alocado dinâmicamente que contém a string do IP
*/
char* inputServerIP()
{
    char* srv_ip = (char*)calloc(INET_ADDRSTRLEN,sizeof(char));
    fgets(srv_ip, INET_ADDRSTRLEN, stdin);
    srv_ip[strcspn(srv_ip,"\n")] = '\0';
    if(DEBUG){printf("%s", srv_ip);}

    return srv_ip;
}

/*
    Cria um socket para TCP, o nomeia (faz o bind) e retorna seu descritor.
    O socket se conecta à `INADDR_ANY` pela porta definida em `CNCT_PORT`

    params:
        const int side ->> Lado da conexão (`SRV_SIDE` ou `CLI_SIDE`)

    returns:
        int socketFD ->> Descritor do socket já nomeado
*/
int createTCPSocket(const int side)
{

    int socketFD =  socket(AF_INET, SOCK_STREAM, 0);                      // Cria uma socket preparada para o TCP (família AF_INET e do tipo STREAM)

    // "Nomeia" o socket (o atrela à um endereço)
    /*
        Analisemos a struct sockadddr_in =>
            .sin_addr.s_addr ->> Endereço da máquina a se conectar (no caso, INADDR_ANY especifica que pode se conectar à qualquer IP desta máquina local seja localhost, o IP na rede local ou o IP externo)
            .sin_family ->> Especifica a família de protocolos (mesma do Socket criado anteriormente)
            .sin_port ->> Porta à qual o socket deve se conectar (mesma que o servidor)
    */
    struct sockaddr_in sockAddr;
    
    // Atua de acordo com o lado da conexão (Servidor ou cliente)
    switch (side)
    {
    case SRV_SIDE:
        {
            
            sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            break;
        }
    
    case CLI_SIDE:
        {
            printf("Digite o IP do servidor: ");
            char* srv_ip = inputServerIP();
            sockAddr.sin_addr.s_addr = htonl(*srv_ip);
            free(srv_ip);
            break;
        }
    
    default:
        {
            printf("Especifique qual o lado da conexão (SRV_SIDE ou CLI_SIDE deste socket)");
            return -1;
        }
    }
    
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(CNCT_PORT);

    int bindRes = bind(socketFD, (struct sockaddr*)&sockAddr, sizeof(sockAddr));  // Liga o socket criado à seu endereço

    if(DEBUG)
    {
        if(bindRes == -1)
        {
            printf("Ocorreu um erro ao fazer o bind do socket");
            return -1;
        }
        else
        {
            printf("Bind ocorreu sem problemas");
        }
    }

    return socketFD;
}