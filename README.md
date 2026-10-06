# Sistema Cliente-Servidor via Sockets (TCP/UDP)

Trabalho prático desenvolvido para a disciplina **SSC0641 - Redes de Computadores** do Instituto de Ciências Matemáticas e de Computação da USP (ICMC/USP). O objetivo da atividade é implementar uma aplicação concorrente utilizando sockets de baixo nível (sem bibliotecas externas) em linguagem C/C++.

## Integrantes do Grupo

| Nome | NUSP |
| :--- | :--- |
| **Bianca Duarte Batista Lacerda** | 15443221 |
| **Lucas Soares Leite Santos** | 15472162 |
| **Murilo de Lima Mariano** | 15480898 |
| **Pedro Luis de Alencar Ribeiro** | 15590852 |
| **Vinicius Reis Gonçalves** | 15491921 |

---

## Especificações do Ambiente
* **Sistema Operacional:** Linux (Ubuntu 22.04.5 LTS )
* **Compilador:** `gcc` (compatível com Padrão C/C++)
* **Gerenciador de Build:** `Makefile`

---

## Funcionalidades e Arquitetura

A aplicação funciona como um chat em grupo, onde vários clientes, se conectam à um servidor, que é 
responsável por receber, gerenciar e espalhar as mensagens do remetente de acordo com o contexto.

Para tornar a aplicação robusta e funcional, seguiu-se os seguintes procedimentos:

1. **Comunicação via Sockets:** Implementação nativa de sockets (bibliotecas padrão do Linux como `<sys/socket.h>`, `<netinet/in.h>`, etc.), garantindo a troca de mensagens confiável e sem o uso de frameworks externos.

2. **Gerenciamento de Múltiplas Conexões (Concorrência):** O servidor emprega gerenciamento de threads para atender a múltiplos clientes de forma simultânea.

3. **Tratamento de Erros e Robustez:** Verificações integradas de falhas de conexão, fechamento abrupto de clientes e gerenciamento correto do ciclo de vida dos sockets (criação, bind, listen, accept/connect, envio/recepção e encerramento seguro).

A aplicação têm 2 funcionalidades principais, o chat em grupo global e o sussuro, onde um usuário envia
mensagem à outro único usuário.

Para que o usuário possa sussurrar à outro, ele deve digitar `/w 'nome_outro_user'` no terminal, assim 
firmando a conexão e tornando as mensagens enviadas visíveis apenas para o usuário desejado.

Para retornar à enviar mensagens globalmente, basta com que o usuário digite `/s` no terminal.

Abaixo, estão estes comandos e mais outros relevantes, com uma breve descrição:

### Comandos:

|Comando|Ação|
|:--|:--|
| *'/w (nome_user)'* | Envia mensagens somente ao usuário de nome (nome_user), até retornar ao chat global |
| *'/s'* | Retorna ao chat global |
| *'/q'* | Sai do chat |

---

## Estrutura do Repositório

```text
├── src/gchatsrv.c # Código-fonte da aplicação cliente
├── src/gchatcli.c # Código-fonte da aplicação servidor
├── lib/           # Arquivos de header do projeto
├── Makefile       # Automação de compilação
└── README.md      # Documentação auxiliar do projeto
```

---

## Instruções de compilação e execução

Para compilar todos os arquivos da aplicação, basta utilizar a diretiva `make all`, ou somente `make`.

Para executar o programa do servidor, utilize a diretiva `make run-srv`.
Análogamente, utilize a diretiva `make run-cli` para executar o cliente.

A seguir, uma tabela com outras diretivas e suas funções:

|Diretiva|Ação|
|:--|:--|
|`all`| Compila todos os arquivos necessários |
| `clean` | Remove os arquivos binários e de objeto |
| `fresh` | Combina `clean` e `all` |
| `run-srv` | Executa o servidor |
| `run-cli` | Executa o cliente |
| `debug-srv` | Executa o servidor no modo debugging |
| `debug-cli` | Executa o cliente no modo debugging |
| `valgrind-srv` | Executa o servidor verificando os memory leaks |
| `valgrind-cli` | Executa o cliente verificando os memory leaks |
