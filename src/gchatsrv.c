#include "gchat.h"

// Inicia o servidor
int main()
{
    // Cria uma socket preparada para o TCP (família AF_INET e do tipo STREAM)
    int srvSockFD =  socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in* srvSockAddr = createSocketAddrIPV4(SRV_SIDE, srvSockFD);
    if(srvSockAddr == NULL){ exit(-1); }

    int opt = 1;
    setsockopt(srvSockFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Começa a escutar conexões
    listen(srvSockFD, MAX_CONN);

    // Cria a lista de usuários e inicia o chat
    USER_LIST* list = createUserList();
    startChat(list, srvSockFD);

    close(srvSockFD);
    free(srvSockAddr);
    free(list);

    return 0;
}