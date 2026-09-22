#ifndef USER_UTILS
#define USER_UTILS

// Estrutura que define um usuário no chat em grupo
typedef struct _user 
{
    char* name; // Nome do usuário
    int id;     // Id do usuário
}USER;


// Estrutura que define um node usuário para a árvore que os 'armazenará'
typedef struct _user_node
{
    USER* user;                 // Usuário
    struct _user_node* left;    // Filho da esquerda
    struct _user_node* right;   // Filho da direita
    struct _user_node* ant;     // Nó pai
}USER_NODE;

// Estrutura que define uma árvore de usuários
typedef struct _user_tree
{
    USER_NODE* root;    // Raíz da árvore
    int num_users;      // Número de usuários já armazenados
}USER_TREE;

/*
 * Cria um usuário
 *   params:
 *       char name ->> Nome do usuário
 *   returns:
 *       USER* ->> Ponteiro para estrutura de usuário com id -1 e nome passado como parâmetro
 */
USER* createUser(char* name);

/**
 * Aloca memória para uma árvore binária de nós do tipo USER_NODE (contém o número de usuários 
 * e a raíz da árvore)
 * 
 * returns:
 *  USER_TREE* ->> Ponteiro para a estrutura da árvore binária, com raiz NULL e num_users 0
 */
USER_TREE* createUserTree();

/**
 * Insere um usuário na árvore de usuários
 *  params:
 *      USER_TREE* tree ->> Árvore em que o usuário será inserido
 *      USER* user ->> Usuário a ser inserido
 */
void insertUser(USER_TREE* tree, USER* user);

/**
 * Procura um usuário por seu nome
 *  params:
 *      USER_TREE* tree ->> Árvore à realizar a busca
 *      const char* user_name ->> Nome do usuário
 *  returns:
 *      USER* ->> Ponteiro para o usuário encontrado (ou NULL caso não)
 */
USER* searchUser(USER_TREE* tree, const char* user_name);

/**
 * Libera a memória de toda uma árvore de usuários
 *  params:
 *      USER_NONDE* root ->> Raíz da árvore
 */
void destroyTree(USER_NODE* root);

#endif