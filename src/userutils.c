#include "gchat.h"

/*
 * Cria um usuário
 *   params:
 *       char name ->> Nome do usuário
 *   returns:
 *       USER* ->> Ponteiro para estrutura de usuário com id -1 e nome passado como parâmetro
 */
USER* createUser(char* name)
{
    USER* user = (USER*)calloc(1,sizeof(USER));
    user->id = -1;
    user->name = (char*)calloc(strlen(name), sizeof(char));
    user->name = name;
    return user;
}

/**
 * Cria um nó de usuário, utilizado para salvá-lo na árvore de usuários
 *  params:
 *      USER* user ->> Usuário a ser salvo
 *  returns:
 *      USER_NODE* ->> Ponteiro para estrutura do tipo USER_NODE (Nó de árvore binária)
 */
USER_NODE* createUserNode(USER* user)
{
    USER_NODE* user_node = (USER_NODE*)calloc(1,sizeof(USER_NODE));
    user_node->left = user_node->right = user_node->ant = NULL;
    user_node->user = user;
}

/**
 * Aloca memória para uma árvore binária de nós do tipo USER_NODE (contém o número de usuários 
 * e a raíz da árvore)
 * 
 * returns:
 *  USER_TREE* ->> Ponteiro para a estrutura da árvore binária, com raiz NULL e num_users 0
 */
USER_TREE* createUserTree()
{
    USER_TREE* tree = (USER_TREE*)calloc(1,sizeof(USER_TREE));
    tree->root = NULL;
    tree->num_users = 0;

    return tree;
}

/**
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
 * Libera a memória de toda uma árvore de usuários
 *  params:
 *      USER_NONDE* root ->> Raíz da árvore
 */
void destroyTree(USER_NODE* root)
{
    if(root == NULL){return;}

    destroyTree(root->left);
    destroyTree(root->right);

    destroyUser(root->user);
    free(root);
}

/**
 * Insere um usuário na árvore de usuários
 *  params:
 *      USER_TREE* tree ->> Árvore em que o usuário será inserido
 *      USER* user ->> Usuário a ser inserido
 */
void insertUser(USER_TREE* tree, USER* user)
{
    if(tree->root == NULL)
    {
        tree->root = createUserNode(user);
    }
    else
    {
        USER_NODE* p = tree->root;
        USER_NODE* ant = NULL;
        int wasLeft = 0;
        while(p!=NULL)
        {
            ant = p;
            if(strcmp(user->name,p->user->name) < 0)
            {
                p = p->left;
                wasLeft = 1;
            }
            else if(strcmp(user->name,p->user->name) > 0)
            {
                p = p->right;
                wasLeft = 0;
            }
            else
            {
                printf("Usuário já cadastrado !\n");
                return;
            }
        }
        switch (wasLeft)
        {
        case 1:
            {
                ant->left = createUserNode(user);
                break;
            }
        case 0:
            {
                ant->right = createUserNode(user);
                break;
            }
        }
    }
    tree->num_users++;
    user->id = tree->num_users - 1;
}

/**
 * Busca o nó de usuário dentro da árvore, e o retorna caso encotrar
 *  params:
 *      USER_TREE* tree ->> Árvore a realizar a busca
 *      const char* user_name ->> Nome do usuário procurado
 *  returns:
 *      USER_NODE* ->> Nó da árvore do usuário correspondente
 */
USER_NODE* searchUserNode(USER_TREE* tree, const char* user_name)
{
    USER_NODE* p = tree->root;
    while(p != NULL)
    {
        if(strcmp(user_name, p->user->name)){ return p; }
        else if(strcmp(user_name,p->user->name) < 0){ p = p->left; }
        else if(strcmp(user_name,p->user->name) > 0){ p = p->right; }
    }

    return NULL;
}

/**
 * Procura um usuário por seu nome
 *  params:
 *      USER_TREE* tree ->> Árvore à realizar a busca
 *      const char* user_name ->> Nome do usuário
 *  returns:
 *      USER* ->> Ponteiro para o usuário encontrado (ou NULL caso não)
 */
USER* searchUser(USER_TREE* tree, const char* user_name)
{
    USER_NODE* user_node =  searchUserNode(tree, user_name);
    if(user_node != NULL){ return user_node->user; }
    return NULL;
}

// void removeUser(USER_TREE* tree, const int user_id)
// {
//     USER_NODE* user_node = searchUserNode(tree, user_id);
//     // TODO - Implementar remoção de usuário
// }