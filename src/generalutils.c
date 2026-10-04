#include "gchat.h"


/**
 * Recupera o argumento de um comando do chat
 *  params:
 *      char* command ->> Comando inteiro "/'X' 'arg'"
 *  returns:
 *      char* ->> Argumento do comando (nome ou id) | NULL caso não tenha. ("/q" ou "/r")
 */
char* getCommandArg(char* command)
{
    strtok(command, " ");
    char* arg = strtok(NULL, " ");
    if(arg != NULL) { arg[strcspn(arg,"\n")] = '\0'; }
    
    return arg;
}

/**
 * Exibe os dados do usuário
 *  params:
 *      USER* user ->> Usuário a ter dados exibidos
 */
void printUserData(USER* user)
{
    puts("------------------");
    printf("ID ->> %i:\n", user->id);
    printf("Nome ->> %s\n", user->name);
    printf("Socket ->> %i\n", user->sockFD);
    puts("------------------");
}

/**
 * Exibe toda a lista de usuários
 *  params:
 *      USER_LIST* list ->> Lista a ser exibida
 */
void printListData(USER_LIST* list)
{
    USER_NODE* p = list->head;
    printf("==== Lista de usuários ====\n\n");
    while(p != NULL)
    {
        printUserData(p->user);
        p = p->next;
    }
    printf("\n==== Fim da lista ====");
}

/**
 * Exibe uma mensagem, com todas as informações relevantes
 *  params:
 *      MESSAGE* message ->> Mensagem a ser exibida
 */
void printMsg(MESSAGE* message)
{
    puts("=======================");
    printf("Remetente: - \n");
    printUserData(&message->sender);
    printf("Mesagem: %s\nWhisp_to: %i\n", message->message, message->to_id);
    puts("=======================");
}

/**
 * Pega um input da tela
 *  params:
 *      const int len ->> Tamanho do input
 *  returns:
 *      char* ->> Texto recebido
 */
char* getStringInput(const int len)
{
    char* text = (char*)calloc(len,sizeof(char));
    fgets(text, len, stdin);
    text[strcspn(text,"\n")] = '\0';

    return text;
}
