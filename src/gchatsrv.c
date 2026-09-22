#include "gchat.h"


void resolveCommands(MESSAGE* message, USER_TREE* tree)
{
    if(strcmp(message->message, "/q") == 0)
    { 
        // TODO - Implementar remoção de usuário
        sprintf(message->message, "O usuário %s saiu da conversa !\0", message->username);
        sprintf(message->username, "Servidor\0");
    }
    else if(strcmp(message->message,"/s") == 0){ message->to_id = -1; }
    else if(message->message[1] == 'w')
    {
        char wName[NAME_SIZE];
        strtok(message->message, " ");
        strcpy(wName, strtok(message->message, NULL));

        USER* user = searchUser(tree, wName);
        if(user == NULL){ strcpy(message->message,"Usuário não encontrado\0"); }
        else{ message->to_id = user->id; }
    }
}

int main()
{
    // Cria uma socket preparada para o TCP (família AF_INET e do tipo STREAM)
    int srvSockFD =  socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(srvSockFD,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    socklen_t addrSize = sizeof(struct sockaddr_in);

    struct sockaddr_in* srvSockAddr = createSocketAddrIPV4(SRV_SIDE, srvSockFD);

    printf("Escutando...\n");
    listen(srvSockFD, MAX_CONN);

    printf("\nAguardando conexão...\n");
    struct sockaddr_in* cliSockAddr = (struct sockaddr_in*)calloc(1,sizeof(struct sockaddr_in));
    int cliSockFD = accept(srvSockFD, (struct sockaddr*)cliSockAddr, &addrSize);
    printf("\nConexão aceita !\n");

    USER_TREE* tree = createUserTree();

    while (1)
    {
        USER* user;
        MESSAGE* rcv_msg = (MESSAGE*)calloc(1,sizeof(MESSAGE));
        recv(cliSockFD, rcv_msg, sizeof(*rcv_msg), 0);

        if(rcv_msg->attach_id[0] == 0)
        {
            user = createUser(rcv_msg->username);
            insertUser(tree, user);
            rcv_msg->attach_id[0] = 1;
            rcv_msg->attach_id[1] = user->id;
        }
        
        send(cliSockFD, rcv_msg, sizeof(*rcv_msg), 0);
        free(rcv_msg);
        
    }

    close(cliSockFD);
    close(srvSockFD);
    free(srvSockAddr);
    free(cliSockAddr);
    

    return 0;
}