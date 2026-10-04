#include "gchat.h"

// Inicializa o cliente
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
        free(cliSockAddr);
        close(cliSockFD);
        return -1;
    }

    // Registra o usuário no servidor
    USER* user = registerUser(cliSockFD);

    // Inicializa o whisp, como broadcast
    int whisp_to = -1;

    // Começa a receber mensagens do servidor
    startReceiving(user, cliSockFD, &whisp_to, cliSockAddr);

    // Envia mensagens ao servidor
    while(1)
    {
        cliSndMsg(user, cliSockFD, &whisp_to);
    }

    return 0;
}