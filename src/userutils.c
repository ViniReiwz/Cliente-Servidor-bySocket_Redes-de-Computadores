#include "gchat.h"

/*
 * Cria um usuário
 *   params:
 *      const char name ->> Nome do usuário
 *   returns:
 *      USER* ->> Ponteiro para estrutura de usuário com id -1 e nome passado como parâmetro
 */
USER* createUser(const char* name)
{
    USER* user = (USER*)calloc(1,sizeof(USER));
    user->id = user->sockFD = -1;
    strcpy(user->name, name);
    return user;
}

USER* copyUser(USER* user)
{
    USER* copy = (USER*)calloc(1,sizeof(USER));
    copy->id = user->id;
    strcpy(copy->name,user->name);
    copy->sockFD = user->sockFD;
    return copy;
}

/**
 * Cria uma lista de usuários
 *  returns:
 *  USER_LIST* ->> Ponteiro para estrutura da lista, com head e tail == NULL
 */
USER_LIST* createUserList()
{
    USER_LIST* list = (USER_LIST*)calloc(1,sizeof(USER_LIST));
    list->head = list->tail = NULL;
    list->num_users = 0;
    return list;
}

/**
 * Cria um nó de usuário, estrutura utilizada para armazená-los na lista
 *  params:
 *      USER* user ->> Usuário a ser atrelado à um nó.
 *  returns:
 *      USER_NODE* ->> Nó de usuário com next e ant == NULL e usuário sendo o ponteiro para USER (passado por parâmetro).
 */
USER_NODE* createUserNode(USER* user)
{
    USER_NODE* usr_node = (USER_NODE*)calloc(1,sizeof(USER_NODE));
    usr_node->user = copyUser(user);
    usr_node->next = usr_node->ant = NULL;
    return usr_node;
}

/**
 * Insere o usuário na lista
 *  params:
 *      USER_LIST* list ->> Lista de usuários
 *      USER* user ->> Usuário a ser inserido
 */
void insertUser(USER_LIST* list, USER* user)
{
    list->num_users++;
    user->id = list->num_users - 1;
    USER_NODE* usr_node = createUserNode(user);
    if(list->head == NULL)
    {
        list->head = list->tail = usr_node;
    }
    else
    {
        list->tail->next = usr_node;
        usr_node->ant = list->tail;
        list->tail = usr_node;
    }
}

/**
 * Remove um usuário de uma lista de usuários
 *  params:
 *      USER_LIST* list ->> Lista em que o usuário está
 *      USER* user ->> Usuário a ser removido
 */
void removeUser(USER_LIST* list, USER* user)
{
    USER_NODE* p = list->head;
    while(p!=NULL)
    {
        if(p->user->id == user->id) 
        { 
            USER_NODE* ant = p->ant;
            if(ant == NULL){ list->head = p->next; }
            else{ ant->next = p->next; }
            if(p->next == NULL) { list->tail = p->ant; }
            else{ p->next->ant = ant; }

            return;
        }
        p = p->next;
    }
}

/**
 * 
 * Libera a memória alocada para um usuário
 *  params:
 *      USER* user ->> Usuário a ser 'deletado'
 */
void destroyUser(USER* user)
{
    free(user->name);
    free(user);
}

/**
 * Libera a memória de um user node.
 *  params:
 *      USER_NODE* usr_node ->> Nó a ser liberado
 */
void destroyUserNode(USER_NODE* usr_node)
{
    destroyUser(usr_node->user);
    free(usr_node);
}

/**
 * Procura um usuário  na lista pelo seu id
 *  params:
 *      USER_LIST* list ->> Lista em que o usuário se encontra
 *      int id ->> Id do usuário desejado
 *  returns:
 *      USER* ->> Usuário encontrado | NULL (em caso de não encontrar na lista)
 */
USER* searchUserByID(USER_LIST* list, int id)
{
    USER_NODE* p = list->head;
    while(p != NULL){ if(p->user->id == id){ return p->user; } p = p->next; }
    return NULL;
    
}

/**
 * Procura um usuário por seu nome
 *  params:
 *      USER_LIST* list ->> Lista em que o usuário se encontra
 *      const char name ->> Nome do usuário a ser procurado
 *  returns:
 *      USER* ->> Usuário encontrado | NULL (em caso de não encontrar na lista)
 */
USER* searchUserByName(USER_LIST* list, const char* name)
{
    USER_NODE* p = list->head;
    while(p != NULL)
    { 
        if(strcmp(p->user->name, name) == 0)
        { 
            return p->user; 
        } 
        p = p->next; 
    }
    return NULL;
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