#include "gchat.h"


int main()
{
    // Cria uma socket preparada para o TCP (família AF_INET e do tipo STREAM)
    int cliSockFD =  socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in* cliSockAddr = createSocketAddrIPV4(CLI_SIDE, cliSockFD);

    // Conecta o cliente ao endereço especificado em clisSockAddr
    int conRes = connect(cliSockFD, (struct sockaddr*)cliSockAddr, sizeof(*cliSockAddr));
    if(conRes == -1)
    {
        perror("connect");
        printf("Erro ao conectar cliente.\n");
        return -1;
    }

    while(1)
    {
        // Envia uma mensagem teste para saber se a conexão foi feita corretamente.
        char message[1024];
        fgets(message, 1023, stdin);
        message[strcspn(message,"\n")] = '\0';  
        
        int bytesSend =  send(cliSockFD, message, strlen(message), 0);
        if(bytesSend == -1){perror("send");}

        if(strcmp(message,"exit") == 0){break;}



        // Recebe e exibe a mensagem de resposta teste do servidor
        char buffer[1024];
        recv(cliSockFD, buffer, sizeof(buffer),0);
        printf("Resposta ->> %s\n", buffer);
    }

    close(cliSockFD);
    free(cliSockAddr);


    return 0;
}