#include "gchat.h"


int main()
{
    // Cria uma socket preparada para o TCP (família AF_INET e do tipo STREAM)
    int srvSockFD =  socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(srvSockFD,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    socklen_t addrSize = sizeof(struct sockaddr_in);

    struct sockaddr_in* srvSockAddr = createSocketAddrIPV4(SRV_SIDE, srvSockFD);

    printf("Escutando...\n");
    listen(srvSockFD, MAX_CONN);

    printf("\nAguardando conexão...\n");
    struct sockaddr_in* cliSockAddr = (struct sockaddr_in*)calloc(1,sizeof(struct sockaddr_in));
    int cliSockFD = accept(srvSockFD, (struct sockaddr*)cliSockAddr, &addrSize);
    printf("\nConexão aceita !\n");

    char buffer[1024];
    int bytesRec = recv(cliSockFD, buffer, 1023, 0);
    buffer[bytesRec] = '\0';
    printf("Mensagem recebida ->> %s\n", buffer);
    
    char* response = "Resposta a mensagem de teste !!\0";
    int bytesSend = send(cliSockFD, response, strlen(response), 0);
    if(bytesSend == -1){perror("send");}

    close(cliSockFD);
    close(srvSockFD);
    free(srvSockAddr);
    free(cliSockAddr);
    

    return 0;
}