#include "gchat.h"

// Mutex da tela
pthread_mutex_t screen_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Exibe as mensagens recebidas do servidor na tela
 *  params:
 *      MESSAGE* msg ->> Mensagem a ser exibida
 *      USER* user ->> Usuário do cliente
 */
void showMessage(MESSAGE* msg, USER* user)
{
    // Comandos não são exibidos
    if(msg->message[0] == '/') { return; }

    // Trava a tela para impressão e limpa a linha (para retirar possível lixo)
    pthread_mutex_lock(&screen_mutex);
    printf("\r\033[K");

    // Caso a mensagem tenha sido deste usuário
    if (msg->sender.id == user->id)
    { printf("Você: %s\n", msg->message); }

    // Mensagem de broadcast
    else if(msg->to_id == -1)
    { printf("%s: %s\n", msg->sender.name, msg->message); }

    // Mensagem de sussurro
    else
    { printf("%s sussurra para você: %s\n", msg->sender.name, msg->message); }        

    printf(INP_INTERF);
    fflush(stdout);
    pthread_mutex_unlock(&screen_mutex);
}


/**
 * Resolve os comandos no lado do cliente
 *  params:
 *      MESSAGE* msg ->> Mensagem recebida do servidor
 *      USER* user ->> Usuário do cliente
 *      int* whisp_to ->> Ponteiro para o ID de quem deve sussurrar
 */
void cliResolveCommands(MESSAGE* msg, USER* user, int* whisp_to)
{
    char command = msg->message[1];
    switch (command)
    {
        case 'r':
        {
            // Atribui o novo id do usuário, vindo do servidor
            int new_id = atoi(getCommandArg(msg->message));
            user->id = new_id;
            break;
        }

        case 'q':
        {
            // Indica que deve encerrar o programa
            msg->comm = QUIT_COMM;
            break;
        }

        case 'w':
        {
            // Recebe o id do usuário para o qual começou a sussurrar
            int new_w_id = atoi(getCommandArg(msg->message));
            *whisp_to = new_w_id;
            break;
        }

        case 's':
        {
            // Retorna para o chat global (broadcast das mensagens)
            *whisp_to = -1;
            break;
        }
    }
}

/**
 * Rotina para registrar usuário
 *  params:
 *      const int cliSockFD ->> Socket do cliente
 *  returns:
 *      USER* ->> Usuário registrado no servidor
 */
USER* registerUser(const int cliSockFD)
{

    printf("Digite seu nome: ");
    char* user_name = getStringInput(NAME_SIZE);
    USER* user = createUser(user_name);

    free(user_name);

    // Envia mensagens com comando de registro ao servidor
    MESSAGE* first_msg = buildMessage(user, "/r", -1);
    ssize_t byteSend = sendAll(cliSockFD, first_msg, sizeof(MESSAGE));
    if(DEBUG){ printf("Total enviado no registro: %li", byteSend); }    
    free(first_msg);

    // Recebe a mensagem com o ID do usuário
    MESSAGE* rcv_msg = (MESSAGE*)calloc(1,sizeof(MESSAGE));
    ssize_t bytesRec = recvAll(cliSockFD, rcv_msg, sizeof(*rcv_msg));
    if(DEBUG){ printf("Total enviado no registro: %li", bytesRec); }    
    
    // Variável auxiliar
    int w_faker = 1;

    // Resolve o comando
    cliResolveCommands(rcv_msg, user, &w_faker);

    free(rcv_msg);

    return user;
}

/**
 * Rotina de envio de mensagem do cliente
 *  params:
 *      USER* user ->> Usuário do cliente
 *      int cliSockFD ->> Socket do cliente
 *      const int* whisp_to ->> ID do usuário à sussurrar (-1 para broadcast)
 */
void cliSndMsg(USER* user, int cliSockFD, const int* whisp_to)
{
    fflush(stdin);  // Limpa o buffer do teclado

    // Trava a tela para evitar sobreposiçoes de informação (prints sobrepostos)
    pthread_mutex_lock(&screen_mutex);
    printf(INP_INTERF); // Exibe a 'interface' de usuário para enviar mensagem
    pthread_mutex_unlock(&screen_mutex);

    // Lẽ do teclado e constrói a mensagem
    char* text = getStringInput(MSG_SIZE);
    MESSAGE* snd_msg = buildMessage(user, text, *whisp_to);

    free(text);

    // Envia a mensagem ao servidor
    ssize_t bytesSend =  sendAll(cliSockFD, snd_msg, sizeof(MESSAGE));
    if(bytesSend == -1){perror("send");}

    free(snd_msg);
}

/**
 * Função chamada pela thread do cliente, que recebe constantemente mensagens do servidor, até que o cliente 
 * deseje sair do chat
 *  params:
 *      void* args ->> Convertido em CLI_MSG_ARGS* que contém os argumentos necessários para o recebimento 
 *                      de mensagens do cliente
 */
void* rcvMsgThread(void* args)
{
    CLI_MSG_ARGS* msg_args = (CLI_MSG_ARGS*) args;

    while(1)
    {
        MESSAGE* rcv_msg = (MESSAGE*)calloc(1,sizeof(MESSAGE));

        // Recebe a mensagem e resolve os comandos
        recvAll(msg_args->cliSockFD, rcv_msg, sizeof(MESSAGE));
        if(rcv_msg->message[0] == '/')
        { cliResolveCommands(rcv_msg, msg_args->user, msg_args->whisp_to); }

        // Caso seja a  mensagem de saída, encerra o programa
        if(rcv_msg->comm == QUIT_COMM)
        { 
            free(rcv_msg);
            close(msg_args->cliSockFD);
            free(msg_args->user);
            free(msg_args->sockAddr);
            free(msg_args);

            exit(0); 
        }

        // Exibe a mensagem na tela
        showMessage(rcv_msg, msg_args->user);
        free(rcv_msg);
    }
}

/**
 * Começa a receber as mensagens vindas do servidor, com uma thread separada do envio
 *  params:
 *      USER* user ->> Usuário do cliente
 *      int cliSockFD ->> Descritor do socket do cliente
 *      int* whisp_to ->> Variável auxiliar pra decidir para quem enviar as mensagens (broadcast ou usuário 
 *                          uńico)
 *      struct sockaddr_in* sockAddr ->> Endereço do socket
 */
void startReceiving(USER* user, int cliSockFD, int* whisp_to, struct sockaddr_in* sockAddr)
{
    CLI_MSG_ARGS* msg_args = (CLI_MSG_ARGS*)calloc(1,sizeof(CLI_MSG_ARGS));

    msg_args->cliSockFD = cliSockFD;
    msg_args->user = user;
    msg_args->whisp_to = whisp_to;
    msg_args->sockAddr = sockAddr;

    pthread_t thread;
    pthread_create(&thread,NULL,rcvMsgThread,msg_args);
    pthread_detach(thread);
}