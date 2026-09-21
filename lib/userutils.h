#ifndef USER_UTILS
#define USER_UTILS

typedef struct _user 
{
    char* name;
    int id;
}USER;

typedef struct _messsage 
{
    USER* user;
    char* message;
    int to_id;
}MESSAGE;

typedef struct _user_node
{
    USER* user;
    struct _user_node* left;
    struct _user_node* right;
    struct _user_node* ant;
}USER_NODE;

typedef struct _user_tree
{
    USER_NODE* root;
    int num_users;
}USER_TREE;

void destroyUser(USER* user);

USER* createUser(const char* name);

#endif