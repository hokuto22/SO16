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

static char pathf[] = "/.Backup/FIFOs/fifo";
static int ppid = 0;
static int fifo = 0;
static char *uname;
static char *dir;
static int reg = 0;
static int log = 0;


void rotinaCliente(int sinal)
{
	int sucesso = 0;
	char login[] = "Login realizado sucesso!\n";
	char registo[] = "Conta registada com sucesso!\n";
	char utNaoExiste[] = "Nao existe conta com esse nome!\n";
	char utPassErrada[] = "Password errada!\n";
	char utJaExiste[] = "Utilizador com esse nome ja existe!\n";
	char desligado[] = "Servidor desligado!\n";
	switch (sinal)
	{
    
		case SIGUSR1: 
				printf("ficheiro copiado\n");
				pause();
				break;
		
		case SIGUSR2:
				printf("ficheiro recuperado\n");
				pause();
			break;

		case SIGFPE:
				write(1, utNaoExiste, sizeof(utNaoExiste));
			break;

		case SIGTSTP:
				write(1, utPassErrada, sizeof(utPassErrada));
			break;

		case SIGABRT:
				write(1, utJaExiste, sizeof(utJaExiste));
			break;
		
		case SIGINT:
				write(1, registo, sizeof(registo));
				reg++;
			break;		
		
		case SIGCONT:
			break;	

		case SIGTERM:
					write(1, login, sizeof(login));
					log++
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
		readLine(0, pedido, sizeof(pedido));
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

void sendMessage(int id, char **info, int in, int num)
{
	int i = 0, n = 0, response = 0, pid = 0;
	char send[512] = {0}, receve[512] = {0}; /* Tamanho máximo das mensagens 512 bytes */
	char *error = NULL, *res = NULL, *ff = NULL, pidC[128];
	
	ff=strdup(getenv("HOME"));
	strcat(ff, "/.Backup/FIFOs/fifo");
	pid = ppid;
	sprintf(pidC, "%d", pid);
	if (id == 1){
	    error = strcpy(send, uname);
	  	error = strcat(send, ":");
		error = strcat(send, info[0]);
	}
	else{
	error = strcpy(send, info[0]);
	error = strcat(send, ":");
	error = strcat(send, info[1]);
	error = strcat(send, ":");
	error = strcat(send, pidC);
	}
	if (error == NULL)
	{
		perror("sendMessage strcat");
		_exit(EXIT_FAILURE);
	}
	for (i = in; i < num; i++)
	{
		error = strcat(send, ":");
		error = strcat(send, info[i]);
		printf("\n%s\n",send);
		if (error == NULL)
		{
			perror("sendMessage strcat 2");
			_exit(EXIT_FAILURE);
		}
	}
	//strcat(ff, pidC);
	// printf("\n%s\n",ff);
	// response = mkfifo(ff, 0666);
 //    if (response < 0)
 //    {
 //    	if (errno != EEXIST)
 //    	{
 //    		perror("sendMessage mkfifo");
 //    		_exit(EXIT_FAILURE);
 //    	}
 //    }
	strcat(send,"\n");
	n = write(fifo, send, strlen(send)); /* Nunca enviar mensagens maiores que 512 bytes */
	if (n < 0)
	{
		perror("Comunication with the Server failed!\n");
		_exit(EXIT_FAILURE);
	}
	close(fifo);
    /*printf("MENSAGEM ENVIADA = [%s]\n", enviar);*/
    pause();
}


/* Envia mensagem contida no argumento "char **informacao", 
com "int num" posições preenchidas, 
para o descritor fd
Argumento informacao[0] indica o tipo de mensagem.
Espera pela resposta por um FIFO criado da seguinte maneira: 
	informacao[0]:pidResposta:Resto da informacao
Retorna a mensagem recebida. */

// char *sendlog(char **info, int in, int num){
// 	int i = 0, n = 0, pid = 0, response = 0;
// 	char send[512] = {0},  receive[512] = {0}; /* Tamanho máximo das mensagens 512 bytes */
// 	char *res = NULL, ff[512], pidC[128];
	
// 	sprintf(ff, "%s", pathf);
// 	pid = ppid;
// 	sprintf(pidC, "%d", pid);
	
// /*	strcpy(send, uname);
// 	strcat(send, ":");
// 	strcat(send, info[0]); */
// 	strcpy(send, info[0]);
// 	strcat(send, ":");
// 	strcat(send, info[1]);
// 	strcat(send, ":");
// 	strcat(send, pidC);
	
// 	for (i = in; i < num; i++){
// 		strcat(send, ":");
// 		strcat(send, info[i]);
// 	}

// 	strcat(ff, pidC);
// 	response = mkfifo(ff, 0666);
//     if (response < 0)
//     {
//     	if (errno != EEXIST)
//     	{
//     		perror("sendlog mkfifo");
//     		_exit(EXIT_FAILURE);
//     	}
//     }
// 	n = write(fifo, send, sizeof(char) * strlen(send)); /* Nunca enviar mensagens maiores que 512 bytes */
// 	if (n < 0)
// 	{
// 		perror("Comunication with the Server failed!\n");
// 		_exit(EXIT_FAILURE);
// 	}
// 	close(fifo);
// 	response = open(ff, O_RDONLY, 0666);

// 	if (response < 0)
// 	{
// 		perror("sendlog open");
// 		_exit(EXIT_FAILURE);
// 	}
// 	n = read(response, receive, sizeof(receive));
// 	if (n < 0)
// 	{
// 		perror("sendlog read");
// 		_exit(EXIT_FAILURE);
// 	}
// 	close(response);
//     /*printf("MENSAGEM ENVIADA = [%s]\n", enviar);*/
// 	res = strdup(receive);
	
// 	return res;
// }


// void sendAction(char **info, int in, int num){
// 	int  fp = 0, i = 0, n = 0, pid = 0;
// 	char send[512] = {0}; /* Tamanho máximo das mensagens 512 bytes */
// 	char pidC[128];
	
// 	pid = ppid;
// 	sprintf(pidC, "%d", pid);

// /*	strcpy(send, uname);
// 	strcat(send, ":");
// 	strcat(send, info[0]);*/
// 	strcpy(send, info[0]);
// 	strcat(send, ":");
// 	strcat(send, info[1]);
// 	strcat(send, ":");
// 	strcat(send, pidC);
	
// 	for (i = in; i < num; i++)
// 	{
// 		strcat(send, ":");
// 		strcat(send, info[i]);
// 	}
	
// 	fp = open("$DIRSO/.Backup/FIFOs/fifo", O_RDONLY);
// 	n = write(fp, send, sizeof(char) * strlen(send)); /* Nunca enviar mensagens maiores que 512 bytes */
// 	if (n < 0)
// 	{
// 		perror("Comunication with the Server failed!\n");
// 		_exit(EXIT_FAILURE);
// 	}
//     /*printf("MENSAGEM ENVIADA = [%s]\n", enviar);*/
// 	close(fp);
// 	pause();
	
// }



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
		sendMessage(1,strArray, 2, i);
	}
}

/* Main do cliente antes de executar a shell propriamente dita */
int main(void)
{
	int opcao = -1, i = 0, sucesso = 0, tam = 0;
	char opcS[12], **mensagem = (char **)malloc(sizeof(char *) * 10), pedido[128], *response;
	const char *env = getenv("HOME");
	char pedePass[] = "Digite a sua password: \n";
	char aviso[] = "Reintroduza uma opcao correta!\n";
	char menu[] = "1 - Login\n2 - Registo\n? - Ajuda\n9 - Sair\n";
	char ligar[] = "A ligar ao servidor...\n";
	char desligado[] = "Servidor desligado!\n";
	char ligado[] = "Ligacao estabelecida.\n";
	char erro[] = "Ocorreu um erro no servidor ao efetuar o pedido!\n";
	
    clrscr(); /* Limpa ecrã */
    signal(SIGUSR1, rotinaCliente); /* Prepara-se para receber SIGPIPE, pois o servidor pode 
   	    não estar ligado */
    signal(SIGTSTP, rotinaCliente);
    signal(SIGABRT, rotinaCliente);
    signal(SIGINT, rotinaCliente);
    signal(SIGTERM, rotinaCliente);
    signal(SIGFPE, rotinaCliente);
   	signal(SIGUSR2, rotinaCliente);
   	signal(SIGCONT, rotinaCliente);
   	signal(SIGPIPE, rotinaCliente); /* Prepara-se para receber SIGPIPE, pois o servidor pode 
   	    não estar ligado */
   	signal(SIGQUIT, rotinaCliente);
    ppid = getpid(); /* Guarda o PID deste processo (pai de todos os outros que serão criados) 
        para criar FIFO de receção com nome "PPID" */
	dir = strdup(env);
	strcat(dir,pathf);
	mkfifo(dir, 0666);
	fifo=open(dir,O_WRONLY);
	sucesso = write(1, ligar, sizeof(ligar));
	if (sucesso < 0)
	{
		perror("main write");
		_exit(EXIT_FAILURE);
	}
	/* Abrir fifo para enviar mensagens ao servidor */
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
	
	while (1) /* Enquanto o utilizador não escolher exit*/
	{
		dir = strdup(env);
		strcat(dir,pathf);
		mkfifo(dir, 0666);
		fifo=open(dir,O_WRONLY);
		opcao = 0;
	    sucesso = write(1, menu, sizeof(menu)); /* Mostra menu */
	    if (sucesso < 0)
	    {
		    perror("main write 4");
		    _exit(EXIT_FAILURE);
	    }
	    opcS[i] = '\0';
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
				wvUsername(&pedido); /* Pede username */
				mensagem[2] = strdup(pedido);
				sucesso = write(1, pedePass, sizeof(pedePass)); /* Pede pass */
				if (sucesso < 0)
				{
					perror("main write 5");
					_exit(EXIT_FAILURE);
				}
				readLine(0, pedido, sizeof(pedido));
				mensagem[3] = strdup(pedido);
				uname = strdup(mensagem[2]);
				sendMessage (0,mensagem, 2, 4);
				if (log){
						shell();
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
				sendMessage(0,mensagem, 2, 4);
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
