#ifndef CONSTS_H
#define CONSTS_H

// Definição das constantes =================================================================================

#define CNCT_PORT 8080  // Porta de conexão (cliente e servidor)

#define SRV_SIDE 1      // Constantes auxiliares para definir se o socket é do lado do servidor
#define CLI_SIDE 2      // ou do cliente

#define MSG_SIZE 1024   // Tamanho máximo dos textos dos usuários
#define NAME_SIZE 50    // Tamanho do nome do usuário

#define DEBUG 0         // Variável de DEBUG ->> Ative para prints auxiliares na tela

#define MAX_CONN 10     // Número máximo de usuários (conexões) simultâneos

#define QUIT_COMM -2    // Comando que indica que o cliente deve sair do chat e o server encerrar a thread respectiva
#define NONE_COMM 0     // Comando para não fazer nada

// ==========================================================================================================

#endif