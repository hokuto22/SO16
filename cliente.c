#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include "utils.h"
#include "treeA.h"

static char pathf[] = "$DIRSO/.Backup/FIFOs/";
static int ppid = 0;
static int fifo = 0;
static char *uname;
//static char *files[1024] = {0};
//static int nfiles = 0;

#define DIRSO = getenv("DIRSO")

void rotinaShell(int sinal)
{
	int sucesso = 0;
	char desligado[] = "Servidor desligado!\n";
	switch (sinal)
	{
		case SIGUSR1: 
				printf("ficheiro copiado\n");
					//printf("%s: copiado\n", files[nfiles]);
					//nfiles++;
					pause();
				break;
		
		case SIGUSR2:
				printf("ficheiro recuperado\n");
					//printf("%s: recuperado\n", files[nfiles]);
					pause();
			break;
			
		case SIGCONT:
					 //files[1024] = 0;
					 //nfiles = 0;
			break;	

		case SIGTERM:
					printf("Algo correu mal\n");
					//files[1024] = 0;
					//nfiles = 0;
			break;

		case SIGQUIT: /* Ignorar SIGQUIT */
			break;
			
		case SIGPIPE:
			sucesso = kill(ppid, SIGKILL); /* Matar processo */
			sucesso = write(1, desligado, sizeof(desligado));
			if (sucesso < 0)
			{
				perror("rotinaShell write 3");
				_exit(EXIT_FAILURE);
			}
			_exit(1);
	}
}

/* Valida o nome do utilizador: 
retorna 0 em caso de sucesso, 
retorna 1 em caso de insucesso. */
int vUsername(char *username)
{
	int tam = 0, i = 0;
	char c = 0;
	
	tam = strlen(username);
	if (tam < 1)
		return 1;
	for (i = 0; i < tam; i++)
	{
		c = username[i];
		if ((c < '0' || c > 'z') || (c > '9' && c < 'A'))
			return 1;
	}
	
	return 0;
}

/* Ciclo que espera que digite um username válido */
void wvUsername(char *pedido)
{
	int valido = 0, n = 0, sucesso = 0;
	char pedeNome[] = "Digite o seu nome: \n", invalido[] = "Username inválido!\n";
	
	do
	{
    	n = write(1, pedeNome, sizeof(pedeNome));
		if (n < 0)
		{
			perror("wvUsername write");
		    _exit(EXIT_FAILURE);
		}
		readLine(0, pedido, sizeof(*pedido));
		valido = vUsername(pedido);
		if (valido != 0)
		{
			sucesso = write(1, invalido, sizeof(invalido));
			if (sucesso < 0)
			{
				perror("wvUsername write 2");
				_exit(EXIT_FAILURE);
			}
		}
	} while (valido != 0);
}


/* Envia mensagem contida no argumento "char **informacao", 
com "int num" posições preenchidas, 
para o descritor fd
Argumento informacao[0] indica o tipo de mensagem.
Espera pela resposta por um FIFO criado da seguinte maneira: 
	informacao[0]:pidResposta:Resto da informacao
Retorna a mensagem recebida. */

char *sendlog(char **info, int in, int num){
	int i = 0, n = 0, pid = 0, response = 0;
	char send[512] = {0},  receive[512] = {0}; /* Tamanho máximo das mensagens 512 bytes */
	char *res = NULL, ff[512], pidC[128];
	
	sprintf(ff, "%s", pathf);
	pid = ppid;
	sprintf(pidC, "%d", pid);
	
	strcpy(send, uname);
	strcat(send, ":");
	strcat(send, info[0]);
	strcpy(send, info[0]);
	strcat(send, ":");
	strcat(send, info[1]);
	strcat(send, ":");
	strcat(send, pidC);
	
	for (i = in; i < num; i++){
		strcat(send, ":");
		strcat(send, info[i]);
	}

	strcat(ff, pidC);
	response = mkfifo(ff, 0666);
    if (response < 0)
    {
    	if (errno != EEXIST)
    	{
    		perror("sendlog mkfifo");
    		_exit(EXIT_FAILURE);
    	}
    }
	n = write(fifo, send, sizeof(char) * strlen(send)); /* Nunca enviar mensagens maiores que 512 bytes */
	if (n < 0)
	{
		perror("Comunication with the Server failed!\n");
		_exit(EXIT_FAILURE);
	}
	close(fifo);
	response = open(ff, O_RDONLY, 0666);

	if (response < 0)
	{
		perror("sendlog open");
		_exit(EXIT_FAILURE);
	}
	n = read(response, receive, sizeof(receive));
	if (n < 0)
	{
		perror("sendlog read");
		_exit(EXIT_FAILURE);
	}
	close(response);
    /*printf("MENSAGEM ENVIADA = [%s]\n", enviar);*/
	res = strdup(receive);
	
	return res;
}


void sendAction(char **info, int in, int num){
	int  fp = 0, i = 0, n = 0, pid = 0;
	char send[512] = {0}; /* Tamanho máximo das mensagens 512 bytes */
	char pidC[128];
	
	pid = ppid;
	sprintf(pidC, "%d", pid);

	strcpy(send, uname);
	strcat(send, ":");
	strcat(send, info[0]);
	strcpy(send, info[0]);
	strcat(send, ":");
	strcat(send, info[1]);
	strcat(send, ":");
	strcat(send, pidC);
	
	for (i = in; i < num; i++)
	{
		strcat(send, ":");
		strcat(send, info[i]);
	}
	
	fp = open("$DIRSO/.Backup/FIFOs/fifo", O_RDONLY);
	n = write(fp, send, sizeof(char) * strlen(send)); /* Nunca enviar mensagens maiores que 512 bytes */
	if (n < 0)
	{
		perror("Comunication with the Server failed!\n");
		_exit(EXIT_FAILURE);
	}
    /*printf("MENSAGEM ENVIADA = [%s]\n", enviar);*/
	close(fp);
	pause();
	
}



void shell(){
	int ct = 1024, exit = 0, n = 0, i;
	char *line = (char *)malloc(sizeof(char) * ct);
	char comandos[] = "sobucli backup path\nsobucli restore path\n";
	char *token, *strArray[128];

	if(line == NULL){
		perror("shell malloc");
		_exit(EXIT_FAILURE);
	}

	while(exit == 0){
		i = 1;
		n = readLine(0, line, sizeof(line));
		if (n < 0){
		    perror("shell read");
		    _exit(EXIT_FAILURE);
	    }
		
		if(strcmp(line, "exit") == 0){
			exit = 1;
		}
		
		if (strcmp(line, "?") == 0){
			n = write(1, comandos, sizeof(comandos));
			if(n<0){
				perror("shell help");
				_exit(EXIT_FAILURE);
			}
		}
		
		token = strtok(line, " ");

		strArray[0] = strdup(uname);

		while(token != NULL){
			strArray[i] = strdup(token);
			token = strtok(NULL, " ");
			i++;
		}
		sendAction(strArray, 2, i);
	}
}

/* Main do cliente antes de executar a shell propriamente dita */
int main(void)
{
	int opcao = -1, i = 0, sucesso = 0, tam = 0;
	char opcS[12], **mensagem = (char **)malloc(sizeof(char *) * 10), pedido[128], *response;
	char pedePass[] = "Digite a sua password: \n";
	char aviso[] = "Reintroduza uma opcao correta!\n";
	char menu[] = "1 - Login\n2 - Registo\n? - Ajuda\n9 - Sair\n";
	char ligar[] = "A ligar ao servidor...\n";
	char desligado[] = "Servidor desligado!\n";
	char ligado[] = "Ligacao estabelecida.\n";
	char login[] = "Login realizado sucesso!\n";
	char registo[] = "Conta registada com sucesso!\n";
	char utNaoExiste[] = "Nao existe conta com esse nome!\n";
	char utPassErrada[] = "Password errada!\n";
	char utJaExiste[] = "Utilizador com esse nome ja existe!\n";
	char erro[] = "Ocorreu um erro no servidor ao efetuar o pedido!\n";
	
    clrscr(); /* Limpa ecrã */
    signal(SIGUSR1, rotinaShell); /* Prepara-se para receber SIGPIPE, pois o servidor pode 
   	    não estar ligado */
   	signal(SIGUSR2, rotinaShell);
   	signal(SIGCONT, rotinaShell);
   	signal(SIGPIPE, rotinaShell); /* Prepara-se para receber SIGPIPE, pois o servidor pode 
   	    não estar ligado */
   	signal(SIGQUIT, rotinaShell);
    ppid = getpid(); /* Guarda o PID deste processo (pai de todos os outros que serão criados) 
        para criar FIFO de receção com nome "PPID" */
	sucesso = write(1, ligar, sizeof(ligar));
	if (sucesso < 0)
	{
		perror("main write");
		_exit(EXIT_FAILURE);
	}
	fifo = open("$DIRSO/.Backup/FIFOs/fifo", O_WRONLY); /* Abrir fifo para enviar mensagens ao servidor */
	if (fifo < 0)
	{
		sucesso = write(1, desligado, sizeof(desligado));
		if (sucesso < 0)
		{
			perror("main write 2");
			_exit(EXIT_FAILURE);
		}
		_exit(EXIT_SUCCESS);
	}
	sucesso = write(1, ligado, sizeof(ligado));
	if (sucesso < 0)
	{
		perror("main write 3");
		_exit(EXIT_FAILURE);
	}
	
	while (opcao != 9) /* Enquanto o utilizador não escolher exit*/
	{
	    sucesso = write(1, menu, sizeof(menu)); /* Mostra menu */
	    if (sucesso < 0)
	    {
		    perror("main write 4");
		    _exit(EXIT_FAILURE);
	    }
		opcS[0] = '\0'; /* inicializar a \0 para impedir carregar lixo */
	    sucesso = readLine(0, opcS, sizeof(opcS)); /* Lê input do teclado */
	    if (sucesso < 0)
	    {
		    perror("main read");
		    _exit(EXIT_FAILURE);
	    }
	    opcao = atoi(opcS);
	    tam = sizeof(char) * strlen(pedido);
	    for (i = 0; i < tam; i++) /* Limpar buffer para input do teclado */
	    	pedido[i] = '\0';
	    switch (opcao)
	    {
		    case 1: /* login */
				mensagem[0] = strdup("sobucli");
				mensagem[1] = strdup("log");
				wvUsername(pedido); /* Pede username */
				mensagem[2] = strdup(pedido);
				sucesso = write(1, pedePass, sizeof(pedePass)); /* Pede pass */
				if (sucesso < 0)
				{
					perror("main write 5");
					_exit(EXIT_FAILURE);
				}
				readLine(0, pedido, sizeof(pedido));
				mensagem[3] = strdup(pedido);
				response = sendlog (mensagem, 2, 4);
				if (strcmp(response, "US_NOT_REGISTERED") == 0)
				{
					sucesso = write(1, utNaoExiste, sizeof(utNaoExiste));
					if (sucesso < 0)
					{
						perror("main write 7");
						_exit(EXIT_FAILURE);
					}
				}
				else
				if (strcmp(response, "US_WRONG_PASS") == 0)
				{
					sucesso = write(1, utPassErrada, sizeof(utPassErrada));
					if (sucesso < 0)
					{
						perror("main write 8");
						_exit(EXIT_FAILURE);
					}
				}
				else
				if (strcmp(response, "US_LOGIN_SUCCESSFUL") == 0)
				{
					sucesso = write(1, login, sizeof(login));
					if (sucesso < 0)
					{
						perror("main write 8");
						_exit(EXIT_FAILURE);
					}
				uname = mensagem[2];
				shell();

				}
				else
				if (strcmp(response, "ERRO") == 0)
				{
					sucesso = write(1, erro, sizeof(erro));
					if (sucesso < 0)
					{
						perror("main write 9");
						_exit(EXIT_FAILURE);
					}
				}
				break;

		    case 2: /* Registar contas */
		   		mensagem[0] = strdup("sobucli");
		   		mensagem[1] = strdup("reg");
		    	wvUsername(pedido); /* Pede username */
			    mensagem[2] = strdup(pedido);
				sucesso = write(1, pedePass, sizeof(pedePass)); /* Pede pass */
				if (sucesso < 0)
				{
					perror("main write 10");
					_exit(EXIT_FAILURE);
				}
				readLine(0, pedido, sizeof(pedido));
				mensagem[3] = strdup(pedido);
				response = sendlog(mensagem, 2, 4);
    			if (strcmp(response, "OK") == 0) /* Sucesso */
				{
					sucesso = write(1, registo, sizeof(registo));
					if (sucesso < 0)
					{
						perror("main write 11");
						_exit(EXIT_FAILURE);
					}
				}
				else
				if (strcmp(response, "US_ALREADY_EXISTS") == 0) /* Utilizador já existe */
				{
					sucesso = write(1, utJaExiste, sizeof(utJaExiste));
					if (sucesso < 0)
					{
						perror("main write 12");
						_exit(EXIT_FAILURE);
					}
				}
				else
				if (strcmp(response, "ERRO") == 0) /* Erro no servidor */
				{
					sucesso = write(1, erro, sizeof(erro));
					if (sucesso < 0)
					{
						perror("main write 13");
						_exit(EXIT_FAILURE);
					}
				}
    			break;

    		case 9: /* Sair */
	    		return EXIT_SUCCESS;

		    default:
        		sucesso = write(1, aviso, sizeof(aviso));
		        if (sucesso < 0)
			    {
				    perror("main write 17");
				    _exit(EXIT_FAILURE);
			    }
		}
	}
	
	return EXIT_SUCCESS;
}
