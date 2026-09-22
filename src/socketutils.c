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
    if(DEBUG){printf("IP registrado: \n\n%s\n\n", srv_ip);}

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
        case SRV_SIDE:
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
        
        case CLI_SIDE:
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
    strcpy(message->username, user->name);
    strcpy(message->message, text);
    message->to_id = whisp_to;
    if(user->id == -1){ message->attach_id[0] = 0; message->attach_id[1] = -1; }
    else{ message->attach_id[0] = 1; message->attach_id[1] = user->id; }

    return message;
}