#ifndef USER_UTILS
#define USER_UTILS

#include "gchat.h"


// Estrutura que define um usuário no chat em grupo
typedef struct _user 
{
    char name[50]; // Nome do usuário
    int id;     // Id do usuário
    int sockFD  // FD do socket do usuário
}USER;

// Estrutura que define o nó de um usuário na lista que os armazena
typedef struct _user_node
{
    USER* user;                 // Usuário
    struct _user_node* next;    // Próximo nó
    struct _user_node* ant;     // Nó anterior
}USER_NODE; 

// Estrutura auxiliar que representa uma lista de usuários
typedef struct _user_list
{
    USER_NODE* head;            // Cabeça da lista
    USER_NODE* tail;            // Cauda da lista
    int num_users;
}USER_LIST;


/*
 * Cria um usuário
 *   params:
 *       char* name ->> Nome do usuário
 *   returns:
 *       USER* ->> Ponteiro para estrutura de usuário com id -1 e nome passado como parâmetro
 */
USER* createUser(const char* name);

/**
 * Cria uma lista de usuários
 *  returns:
 *  USER_LIST* ->> Ponteiro para estrutura da lista, com head e tail == NULL
 */
USER_LIST* createUserList();

/**
 * Insere o usuário na lista
 *  params:
 *      USER_LIST* list ->> Lista de usuários
 *      USER* user ->> Usuário a ser inserido
 */
void insertUser(USER_LIST* list, USER* user);

/**
 * Remove um usuário de uma lista de usuários
 *  params:
 *      USER_LIST* list ->> Lista em que o usuário está
 *      USER* user ->> Usuário a ser removido
 */
void removeUser(USER_LIST* list, USER* user);

/**
 * Procura um usuário  na lista pelo seu id
 *  params:
 *      USER_LIST* list ->> Lista em que o usuário se encontra
 *      int id ->> Id do usuário desejado
 *  returns:
 *      USER* ->> Usuário encontrado | NULL (em caso de não encontrar na lista)
 */
USER* searchUserByID(USER_LIST* list, int id);

/**
 * Procura um usuário por seu nome
 *  params:
 *      USER_LIST* list ->> Lista em que o usuário se encontra
 *      const char name ->> Nome do usuário a ser procurado
 *  returns:
 *      USER* ->> Usuário encontrado | NULL (em caso de não encontrar na lista)
 */
USER* searchUserByName(USER_LIST* list, const char* name);

/**
 * Exibe os dados do usuário
 *  params:
 *      USER* user ->> Usuário a ter dados exibidos
 */
void printUserData(USER* user);

void printListData(USER_LIST* list);


#endif