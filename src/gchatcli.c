#include "gchat.h"

char* getStringInput(const int len)
{
    char* text = (char*)calloc(len,sizeof(char));
    fgets(text, len, stdin);
    text[strcspn(text,"\n")] = '\0';

    return text;
}

void printMessage(const MESSAGE* message, const int user_id)
{
    if(DEBUG)
    {
        printf("Mensagem recebida do servidor ->> \n");
        printf("\nRemetente: %s\nContéudo: %s\nID do Whisp: %i\n, Attachid[0, 1]: [%i, %i]\n\n", message->username, message->message, message->to_id, message->attach_id[0], message->attach_id[1]);
    }
    else if(message->to_id == user_id || (message->to_id == -1 && message->attach_id[1] != user_id))
    {
        printf("%s: %s\n", message->username, message->message);
    }
}

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

    printf("Digite seu nome: ");
    char* user_name = getStringInput(NAME_SIZE);
    USER* user = createUser(user_name);
    int whisp_to = -1;

    while(1)
    {
        fflush(stdin);

        printf("Você diz: ");
        char* text = getStringInput(MSG_SIZE);

        MESSAGE* snd_msg = buildMessage(user, text, whisp_to);

        free(text);

        int bytesSend =  send(cliSockFD, snd_msg, sizeof(*snd_msg), 0);
        if(bytesSend == -1){perror("send");}

        free(snd_msg);

        // Recebe e exibe a mensagem de resposta teste do servidor
        MESSAGE* rcv_msg = (MESSAGE*)calloc(1,sizeof(MESSAGE));
        recv(cliSockFD, rcv_msg, sizeof(*rcv_msg), 0);
        
        if(rcv_msg->attach_id[0] == 1 && user->id == -1){ user->id = rcv_msg->attach_id[1]; }
        
        printMessage(rcv_msg, user->id);

        free(rcv_msg);
    }

    close(cliSockFD);
    free(cliSockAddr);


    return 0;
}