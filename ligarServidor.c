#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>

/* Função que faz parse dos argumentos de um comando e cria 1 processo filho 
para executar esse comando, ie, semelhante ao system() do stdlib.h: 
retorna 0 se comando bem executado, 
retorna 1 se execvp deu erro. */
int executeCommand(char *command)
{
	int erro = 0, fim = 0, status = 0, arg = 0, tam = 1024, i = 0, ini = 0, res = 0, pid = 0;
	char *argumento, **argumentos = (char **)malloc(sizeof(char *) * tam), **aux = NULL;
	
	if (argumentos == NULL) /* Erro malloc */
	{
		perror("executeCommand malloc");
		_exit(EXIT_FAILURE);
	}
	if (command == NULL || command[i] == '\0') /* Se não escreveram nada */
		return 101;
	while (command[i]) /* Enquanto houver argumentos */
	{ /* Descobre início do argumento */
		for ( ; command[i] == ' ' || command[i] == '\t'; i++) ;
		ini = i;
		/* Descobre fim do argumento */
		for ( ; command[i] != ' ' && command[i] != '\t' && command[i] != '\0'; i++) ;
		fim = i;
		argumento = duplicaString (command, ini, fim);
		if (tam == arg) /* Se o buffer não chega */
		{
			tam *= 2;
			aux = (char **)realloc(argumentos, sizeof(char *) * tam);
			if (aux == NULL) /* Erro realloc */
			{
				perror("executeCommand realloc");
				_exit(EXIT_FAILURE);
			}
			argumentos = aux; /* Atualiza apontador */
		}
		argumentos[arg++] = argumento;
	}
	argumentos[arg] = NULL; /* Último argumento deve ser sempre NULL */
	pid = fork(); /* Cria o processo filho */
	if (pid < 0) /* Erro fork */
	{
		perror("executeCommand fork");
		_exit(1);
	}
	if (pid == 0) /* Processo filho */
	{
		execvp(argumentos[0], argumentos); /* Manda executar o comando */
		_exit(1); /* Avisa processo pai que execvp deu erro */
	} /* Processo pai */
	erro = waitpid(pid, &status, 0); /* Espera pelo processo filho */
	if (erro < 0)
	{
		perror("executeCommand waitpid");
		_exit(EXIT_FAILURE);
	}
	if (WIFEXITED(status) > 0) /* Se processo filho devolveu erro, */
		res = WEXITSTATUS(status); /* descobre o valor do exit do processo filho */
	else
		res = 0; /* Processo filho executou comando com sucesso */
	
	return res; /* Retorna se processo filho executou bem ou se deu erro */
}

/* Rotina do programa que gere o servidor */
void rotinaLigarServidor(int sinal)
{
	Command pgrep = NULL; /* Output do pgrep */
	int lines = 4;
	
	switch (sinal)
	{
		case SIGALRM:
			pgrep = readCommand("pidstat -urdh -C server");
			lines = pgrep->lines;
			if (lines < 4) /* Se tiver menos de 4 linhas é porque o PID não está na tabela */
				executeCommand("./server");
			alarm(3);
			break;
			
		case SIGINT: /* Ignorar SIGINT */
			break;
			
		case SIGQUIT: /* Ignorar SIGQUIT */
			break;
			
		default:
			break;
	}
}

int main(void)
{
	int pid = 0;
	char opcS[12];
	signal(SIGALRM, rotinaLigarServidor);
	signal(SIGINT, rotinaLigarServidor);
	signal(SIGQUIT, rotinaLigarServidor);


	readLine(0, opcS, sizeof(opcS));

	if(strcmp(opcS, "sobusrv") == 0){ 

		pid = fork();
		switch (pid){
			case -1:
				perror("main fork");
				_exit(1);
			
			case 0:
				executeCommand("./server");
				alarm(3);
				while (1)
					pause();
				break;
			
			default:
				_exit(0);
		}
	}	
	
	return EXIT_SUCCESS;
}