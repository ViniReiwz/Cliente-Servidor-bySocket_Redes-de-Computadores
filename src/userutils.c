#include "gchat.h"

USER* createUser(const char name)
{
    USER* user = (USER*)calloc(1,sizeof(USER));
    user->id = -1;
    user->name = (char*)calloc(strlen(name), sizeof(char));
    return user;
}

USER_NODE* createUserNode(USER* user)
{
    USER_NODE* user_node = (USER_NODE*)calloc(1,sizeof(USER_NODE));
    user_node->left = user_node->right = user_node->ant = NULL;
    user_node->user = user;
}

USER_TREE* createUserTree()
{
    USER_TREE* tree = (USER_TREE*)calloc(1,sizeof(USER_TREE));
    tree->root = NULL;
    tree->num_users = 0;

    return tree;
}

void destroyUser(USER* user)
{
    free(user->name);
    free(user);
}

void destroyTree(USER_NODE* root)
{
    if(root == NULL){return;}

    destroyTree(root->left);
    destroyTree(root->right);

    destroyUser(root->user);
    free(root);
}

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
            if(user->id < p->user->id)
            {
                p = p->left;
                wasLeft = 1;
            }
            else if(user->id > p->user->id)
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

USER* searchUserById(USER_TREE* tree, const int user_id)
{
    return searchUserNode(tree, user_id)->user;
}

void removeUser(USER_TREE* tree, const int user_id)
{
    USER_NODE* user_node = searchUserNode(tree, user_id);
    // TODO - Implementar remoção de usuário
}

USER_NODE* searchUserNode(USER_TREE* tree, const int user_id)
{
    USER_NODE* p = tree->root;
    while(p != NULL)
    {
        if(p->user->id == user_id){ return p->user; }
        else if(user_id < p->user->id){ p = p->left; }
        else if(user_id > p->user->id){ p = p->right; }
    }

    return NULL;
}