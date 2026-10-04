#ifndef SRV_UTILS_H
#define SRV_UTILS_H

// Definições de tipo =======================================================================================

// Estrutura auxiliar para passar argumentos à thread de recebimento e envio de mensagens
typedef struct _msg_args
{
    USER_LIST* list;            // Lista de usuários
    ACCEPTED_SOCKET* acpt_sock; // Socket cuja conexão fora aceita (cliente)
}MSG_ARGS;

// ==========================================================================================================

// Funções ==================================================================================================

/**
 * Envia a mensagem resolvida pelo servidor
 *  params:
 *      USER_LIST* list ->> Lista de usuários
 *      MESSAGE* msg ->> Mensagem do servidor
 */
void sndMsg(USER_LIST* list, MESSAGE* msg);

/**
 * Reordena a lista de usuários, atualizando seus IDs de acordo com o número de usuários
 *  presentes no sistema.
 *      params:
 *          USER_LIST* list ->> lista a ser reordenada
 */
void reorderList(USER_LIST* list);

/* Função auxiliar que lida com os 'comandos' implementados no chat, sendo eles iniciados em '/'
 *  params:
 *      MESSAGE* message ->> Mensagem recebida, iniciada em '/'.
 *      USER_LIST* list ->> Lista de usuários
 */
void srvResolveCommands( ACCEPTED_SOCKET* socket, MESSAGE* message, USER_LIST* list, int* comm);

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
MESSAGE* rcvMsg(ACCEPTED_SOCKET* socket, USER_LIST* list,int* comm);

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
void* rcvAndSndMsgThread(void* args);

/** 
 * Função auxiliar que inicia o recebimento e envio de mensagens entre o servidor e os clientes.
 *  params:
 *      ACCEPTED_SOCKET* acpt_sock ->> Estrutura que contém as informações do socket cuja conexão fora aceita
 *      USER_LIST* list ->> Lista de usuários do servidor;
*/
void rcvAndSndMsg(ACCEPTED_SOCKET* acpt_sock, USER_LIST* list);

/**
 * Função auxiliar para facilitar o início do chat em grupo, que aceita a conexão e começa a receber e 
 * enviar mensagens do cliente
 *  params:
 *      USER_LIST* list ->> Lista de usuários;
 *      int srvSockFD ->> Descritor do socket de escuta do servidor.
 */
void startChat(USER_LIST* list, int srvSockFD);

// ==========================================================================================================

#endif