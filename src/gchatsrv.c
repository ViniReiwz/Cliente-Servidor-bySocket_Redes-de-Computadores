#include "gchat.h"

// Estrutura auxiliar para passar os argumentos à thread (lista de users e descritor do server)
typedef struct _boot_args
{
    USER_LIST* list;    // Lista de usuários
    int srvSockFD;      // File descriptor do socket do servidor
}BOOT_ARGS;

// Estrutura auxiliar para passar argumentos à thread de recebimento e envio de mensagens
typedef struct _msg_args
{
    USER_LIST* list;            // Lista de usuários
    ACCEPTED_SOCKET* acpt_sock; // Socket cuja conexão fora aceita (cliente)
}MSG_ARGS;

/**
 * Envia a mensagem resolvida pelo servidor
 *  params:
 *      USER_LIST* list ->> Lista de usuários
 *      MESSAGE* msg ->> Mensagem do servidor
 */
void sndMsg(USER_LIST* list, MESSAGE* msg)
{
    // Se msg->to_id for -1, envia para todos os usuários (chat global). Caso o contrário, envia apenas para o usuário referenciado por to_id.
    if(msg->to_id == -1)
    {
        USER_NODE* p = list->head;
        while(p!=NULL)
        {
            send(p->user->sockFD, msg, sizeof(*msg), 0);
            p = p->next;
        }
    }
    else
    {
        USER* whisp_usr = searchUserByID(list, msg->to_id);
        send(whisp_usr->sockFD, msg, sizeof(*msg), 0);
    }
}

/**
 * Função auxiliar que lida com os 'comandos' implementados no chat, sendo eles iniciados em '/'
 *  params:
 *      MESSAGE* message ->> Mensagem recebida, iniciada em '/'.
 *      USER_LIST* list ->> Lista de usuários
 */
void resolveCommands(MESSAGE* message, USER_LIST* list)
{
    if(strcmp(message->message, "/q") == 0)
    { 
        // TODO - Implementar remoção de usuário
        sprintf(message->message, "O usuário %s saiu da conversa !", message->sender.name);
        sprintf(message->sender.name, "Servidor");
        message->to_id = -1;
    }
    else if(strcmp(message->message,"/s") == 0){ message->to_id = -1; }
    else if(message->message[1] == 'w')
    {
        char wName[NAME_SIZE];
        strtok(message->message, " ");
        strcpy(wName, strtok(NULL, " "));
        if(DEBUG){ printf("Vai sussurar pra ->> %s\n", wName); }
        message->to_id = searchUserByName(list, wName)->id;
    }
    else if(strcmp(message->message,"/r") == 0)
    {
        insertUser(list, &message->sender);
        printUserData(&message->sender);
        message->to_id = message->sender.id;
    }
}

MESSAGE* rcvMsg(ACCEPTED_SOCKET* socket, USER_LIST* list)
{
    MESSAGE* msg = (MESSAGE*)calloc(1,sizeof(MESSAGE));
    int bytesRec = recv(socket->sockFD, msg, sizeof(*msg), 0);
    if(DEBUG){ printf("Bytes recebidos ->> %i\n", bytesRec);  }
    
    if(msg->message[0] == '/'){ resolveCommands(msg, list); }

    return msg;

}

void* rcvAndSndMsgThread(void* args)
{
    MSG_ARGS* msg_args = (MSG_ARGS*)args;

    MESSAGE* rcv_msg = rcvMsg(msg_args->acpt_sock, msg_args->list);    
    sndMsg(msg_args->list, rcv_msg);
    free(rcv_msg);
}

void rcvAndSndMsg(ACCEPTED_SOCKET* acpt_sock, USER_LIST* list)
{
    MSG_ARGS* msg_args = (MSG_ARGS*)calloc(1,sizeof(MSG_ARGS));
    msg_args->acpt_sock = acpt_sock;
    msg_args->list = list;
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, rcvAndSndMsgThread, msg_args);
    pthread_join(thread_id, NULL);

}

void* startChatThread(void* args)
{
    BOOT_ARGS* boot_args = (BOOT_ARGS*)args;

    USER_LIST* list = boot_args->list;
    int srvSockFD = boot_args->srvSockFD;

    while (1)
    {
        ACCEPTED_SOCKET* acpt_sock = accpetIncomingConnection(srvSockFD);
        if(!acpt_sock->wasAccpeted)
        {
            printf("Socket %i não fora aceito ->> Erro: %i\n", acpt_sock->sockFD, acpt_sock->error);
            exit(acpt_sock->error);
        }

        rcvAndSndMsg(acpt_sock, list);

    }
}

void startChat(BOOT_ARGS* boot_args)
{
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, startChatThread, boot_args);
    pthread_join(thread_id, NULL);
}

int main()
{
    // Cria uma socket preparada para o TCP (família AF_INET e do tipo STREAM)
    int srvSockFD =  socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in* srvSockAddr = createSocketAddrIPV4(SRV_SIDE, srvSockFD);
    if(srvSockAddr == NULL){ exit(-1); }

    int opt = 1;
    setsockopt(srvSockFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    listen(srvSockFD, MAX_CONN);

    BOOT_ARGS* boot_args = (BOOT_ARGS*)calloc(1,sizeof(BOOT_ARGS));

    boot_args->srvSockFD = srvSockFD;
    boot_args->list = createUserList();

    startChat(boot_args);

    close(srvSockFD);
    free(srvSockAddr);

    free(boot_args);
    
    return 0;
}