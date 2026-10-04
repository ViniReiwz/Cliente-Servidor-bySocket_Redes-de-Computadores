#include "gchat.h"

/**
 * Envia a mensagem resolvida pelo servidor
 *  params:
 *      USER_LIST* list ->> Lista de usuários
 *      MESSAGE* msg ->> Mensagem do servidor
 */
void sndMsg(USER_LIST* list, MESSAGE* msg)
{
    if(DEBUG){ printMsg(msg); }
    if(msg->to_id == -1)                    // Realiza o broadcast da mensagem (envia à todos os clientes)
    {
        pthread_mutex_lock(&list->mutex);   // Bloqueia a modificação da lista até enviar a mensagem aos clientes
        USER_NODE* p = list->head;
        while(p!=NULL)
        {
            sendAll(p->user->sockFD, msg, sizeof(*msg));
            p = p->next;
        }
        pthread_mutex_unlock(&list->mutex);
    }
    else                                    // Envia para o único cliente referenciado por msg->to_id
    {
        
        USER* whisp_usr = searchUserByID(list, msg->to_id);
        if(whisp_usr == NULL){ return; }
        
        sendAll(whisp_usr->sockFD, msg, sizeof(*msg));
    }
}

/**
 * Reordena a lista de usuários, atualizando seus IDs de acordo com o número de usuários
 *  presentes no sistema.
 *      params:
 *          USER_LIST* list ->> lista a ser reordenada
 */
void reorderList(USER_LIST* list)
{
    // Bloqueia a lista enquanto realiza a modificação, evitando consdições de corrida
    pthread_mutex_t* mutex = &list->mutex;
    pthread_mutex_lock(mutex);

    // Caso vazia, desbloqueia a lista e retorna logo em seguida
    if(list->num_users == 0){ pthread_mutex_unlock(mutex); return; }

    // PErcorre a lista de trás para frente reorganizando os IDs
    USER_NODE* p = list->tail;
    int higher_id = list->num_users - 1;
    while (p != NULL)
    {
        p->user->id = higher_id;

        char text[MSG_SIZE];
        sprintf(text, "/r %i", p->user->id);

        // Envia a mensagem de registro para todos os usuários, com o seu novo ID
        MESSAGE* reorder_msg = buildMessage(p->user, text, p->user->id);
        sendAll(p->user->sockFD, reorder_msg, sizeof(MESSAGE));
        free(reorder_msg);

        higher_id--;
        p = p->ant;
    }

    pthread_mutex_unlock(mutex);
}

/* Função auxiliar que lida com os 'comandos' implementados no chat, sendo eles iniciados em '/'
 *  params:
 *      MESSAGE* message ->> Mensagem recebida, iniciada em '/'.
 *      USER_LIST* list ->> Lista de usuários
 */
void srvResolveCommands( ACCEPTED_SOCKET* socket, MESSAGE* message, USER_LIST* list, int* comm)
{
    // Recupera qual o comando que o usuário deseja realizar, já que vem na forma "/'comando' arg";
    char command = message->message[1];

    // Todo comando retorna inicialmente uma mensagem à quem o enviou, com os argumentos necessários
    message->to_id = message->sender.id;
    switch (command)
    {
        case 'q':   // Quando um usuário deseja sair do chat "/q"
        {
            // Envia a mensagem ao remetente, com o comando de quit, para que encerre o cliente
            USER* user = searchUserByID(list, message->sender.id);
            message->comm = QUIT_COMM;
            sendAll(user->sockFD, message, sizeof(MESSAGE));    

            // Remove o usuário da lista e a reordena
            removeUser(list,user);
            reorderList(list);

            // Faz o broadcast da informação de que o usuário saiu da conversa
            sprintf(message->message, "O usuário %s saiu da conversa !", message->sender.name);
            sprintf(message->sender.name, "[Servidor]");
            message->to_id = -1;
            message->sender.id = -1;

            // Muda para NONE_COMM, para que os outros clientes permaneçam rodando
            message->comm = NONE_COMM;
            // A variável comm é única por thread, logo, indica que essa thread deve ser encerrada no servidor.
            *comm = QUIT_COMM;  

            break;
        }

        case 'w':   // Quando um usuário deseja sussurrar (falar em particular) com outro "/w 'nome'"
        {

            // Recupera o nome do usuário desejado
            char* name = getCommandArg(message->message);
            
            if(name != NULL)
            { 
                USER* w_user =  searchUserByName(list,name); 
                if(w_user != NULL)
                {
                    // Envia o id do usuário desejado ao cliente remetente
                    sprintf(message->message,"/w %i", w_user->id);
                    break;
                }
            }
            
            // Caso seja um usuário inválido
            sprintf(message->message, "Digite um usuário válido !");
            sprintf(message->sender.name, "[Servidor]");
            message->sender.id = -1;
            break;
        }
        
        case 'r':   // Quando um usuário deseja se registrar "/r"
        {
            // Recupera o descritor do socket do usuário, e o insere na lista do servidor
            message->sender.sockFD = socket->sockFD;
            insertUser(list, &message->sender); 
            if(DEBUG) { printUserData(&message->sender); }

            // Envia ao remetente a mensagem com seu id na lista.
            message->to_id = message->sender.id;
            sprintf(message->message, "/r %i", message->to_id);
            break;
        }
    }
}

/**
 * Função que recebe a mensagem do cliente e realiza as tarefas necessárias à depender do que o cliente 
 * enviar.
 *  params:
 *      ACCEPTED_SOCKET* socket ->> Estrutura que contém as informações do socket aceito;
 *      USER_LIST* list ->> lista de usuários
 *      int* comm ->> Variável auxiliar que indica o comando que aquele socket deve realizar;
 * 
 *  returns:
 *      MESSAGE* ->> Mensagem a ser reenviada aos clientes (ou à um único cliente);
 */
MESSAGE* rcvMsg(ACCEPTED_SOCKET* socket, USER_LIST* list,int* comm)
{
    // Recebe a mensagem cinda do cliente
    MESSAGE* msg = (MESSAGE*)calloc(1,sizeof(MESSAGE));
    ssize_t bytesRec = recvAll(socket->sockFD, msg, sizeof(*msg));

    if(DEBUG){ printf("Bytes recebidos ->> %li\n", bytesRec);  }
    
    // Se for um comando, o resolve
    if(msg->message[0] == '/'){ srvResolveCommands(socket, msg, list, comm); }

    // Exibe a mensagem com todas as informações no servidor
    puts("");
    printMsg(msg);
    puts("");

    return msg;

}

/**
 * Função chamada ao criar a thread, uma para cada cliente conectado ao servidor, que recebe e envia 
 * mensagens à estes.
 *  params:
 *      void* args ->> Convertido em MSG_ARGS* que contém os argumentos necessários para o chat funcione     
 *                          corretamente;
 * 
 *  returns:
 *      void* ->> NULL;
 */
void* rcvAndSndMsgThread(void* args)
{
    // Recupera os argumentos vindos da thread principal
    MSG_ARGS* msg_args = (MSG_ARGS*)args;

    // Inicializa comm como NONE_COMM para que os clientes não encerrem automaticamente
    int comm = NONE_COMM;

    // Recebe e envia mensagens até que o cliente deseje sair do chat (comm == QUIT_COMM)
    while(1)
    {
        MESSAGE* rcv_msg = rcvMsg(msg_args->acpt_sock, msg_args->list, &comm);    
        sndMsg(msg_args->list, rcv_msg);

        if(comm == QUIT_COMM){ free(rcv_msg); break; }
        free(rcv_msg);
    }

    // Libera o socket e a memória alocada para tal
    destroyAcptSock(msg_args->acpt_sock);
    free(msg_args);

    return NULL;
}

/** 
 * Função auxiliar que inicia o recebimento e envio de mensagens entre o servidor e os clientes.
 *  params:
 *      ACCEPTED_SOCKET* acpt_sock ->> Estrutura que contém as informações do socket cuja conexão fora aceita
 *      USER_LIST* list ->> Lista de usuários do servidor;
*/
void rcvAndSndMsg(ACCEPTED_SOCKET* acpt_sock, USER_LIST* list)
{
    // Organiza os argumentos para passar à thread
    MSG_ARGS* msg_args = (MSG_ARGS*)calloc(1,sizeof(MSG_ARGS));
    msg_args->acpt_sock = acpt_sock;
    msg_args->list = list;

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, rcvAndSndMsgThread, msg_args);
    pthread_detach(thread_id);
}

/**
 * Função auxiliar para facilitar o início do chat em grupo, que aceita a conexão e começa a receber e 
 * enviar mensagens do cliente
 *  params:
 *      USER_LIST* list ->> Lista de usuários;
 *      int srvSockFD ->> Descritor do socket de escuta do servidor.
 */
void startChat(USER_LIST* list, int srvSockFD)
{   
    // Começa a aceitar conexões e gerenciar o recebimento e envio de mensagens
    while (1)
    {

        // Utiliza select para verificar se close_server fora digitado na tela do terminal (encerra o server seguramente), e se há conexões para lidar com as mensagens
        fd_set fds;
        FD_ZERO(&fds);

        FD_SET(srvSockFD, &fds);
        FD_SET(STDIN_FILENO, &fds);

        int maxFD = srvSockFD > STDIN_FILENO ? srvSockFD : STDIN_FILENO;

        int result = select(maxFD + 1, &fds, NULL, NULL, NULL);

        if (result < 0) 
        {
            perror("select");
            break;
        }

        // Checa se o usuário digitou algo no terminal
        if (FD_ISSET(STDIN_FILENO, &fds))
        {
            char command[100];

            fgets(command, sizeof(command), stdin);
            command[strcspn(command, "\n")] = '\0';

            if (strcmp(command, "close_server") == 0)
            {
                if(list->num_users == 0)
                {
                    printf("Fechando servidor...\n");
                    break;
                }
                else
                {
                    printf("Ainda ha usuários conectados, impossível fechar servidor !\n");
                }
            }
        }

        // Checa se chegou uma nova conexão
        if (FD_ISSET(srvSockFD, &fds))
        {
            ACCEPTED_SOCKET* acpt_sock = accpetIncomingConnection(srvSockFD);

            if (!acpt_sock->wasAccpeted)
            {
                printf("Erro ao aceitar conexão: %i\n",
                       acpt_sock->error);

                free(acpt_sock);
                break;
            }

            rcvAndSndMsg(acpt_sock, list);
        }
    }
}