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

/**
 * Copia um usuário, gerando um novo ponteiro com os mesmos dados.
 *  params:
 *      USER* user ->> Usuário a ser copiado.
 *  returns:
 *      USER* ->> Cópia do usuário referenciado.
 */
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
    pthread_mutex_init(&list->mutex, NULL);
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

    // Copia um usuário porque no contexto do servidor todos os usuários vem das mensagens, que são constatemente liberadas.
    usr_node->user = copyUser(user);    
    usr_node->next = usr_node->ant = NULL;
    return usr_node;
}


/**
 * Libera a memória de um user node.
 *  params:
 *      USER_NODE* usr_node ->> Nó a ser liberado
 */
void destroyUserNode(USER_NODE* usr_node)
{
    free(usr_node->user);
    free(usr_node);
}

/**
 * Insere o usuário na lista
 *  params:
 *      USER_LIST* list ->> Lista de usuários
 *      USER* user ->> Usuário a ser inserido
 */
void insertUser(USER_LIST* list, USER* user)
{
    pthread_mutex_t* mutex = &list->mutex;
    pthread_mutex_lock(mutex);

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

    pthread_mutex_unlock(mutex);
}

/**
 * Remove um usuário de uma lista de usuários
 *  params:
 *      USER_LIST* list ->> Lista em que o usuário está
 *      USER* user ->> Usuário a ser removido
 */
void removeUser(USER_LIST* list, USER* user)
{
    pthread_mutex_t* mutex = &list->mutex;
    pthread_mutex_lock(mutex);

    USER_NODE* p = list->head;
    while(p!=NULL)
    {
        if(p->user->id == user->id) 
        { 
            if(list->head == list->tail)
            {
                list->head = list->tail = NULL;
            }
            else if(p->ant == NULL)
            {
                p->next->ant = NULL;
                list->head = p->next;
            }
            else if(p->next == NULL)
            {
                p->ant->next = NULL;
                list->tail = p->ant;
            }
            else
            {
                USER_NODE* ant = p->ant;
                ant->next = p->next;
                p->next->ant = ant;
            }

            destroyUserNode(p);

            break;  
        }
        p = p->next;
    }
    list->num_users--;
    pthread_mutex_unlock(mutex);
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
    pthread_mutex_t* mutex = &list->mutex;
    pthread_mutex_lock(mutex);

    USER_NODE* p = list->head;
    while(p != NULL)
    { 
        if(p->user->id == id)
        { 
            pthread_mutex_unlock(mutex);
            return p->user; 
        } 
        p = p->next; 
    }
    pthread_mutex_unlock(mutex);
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
    pthread_mutex_t* mutex = &list->mutex;
    pthread_mutex_lock(mutex);

    USER_NODE* p = list->head;
    while(p != NULL)
    { 
        if(strcmp(p->user->name, name) == 0)
        { 
            pthread_mutex_unlock(mutex);
            return p->user; 
        } 
        p = p->next; 
    }
    pthread_mutex_unlock(mutex);
    return NULL;
}