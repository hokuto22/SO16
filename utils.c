#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#define TAM_LINHA 2048

void clrscr(void)
{
	printf("\033[H\033[J"); /* ANSI escape codes (or escape sequences): <=> Non-ISO-standard: printf("\e[H\e[2J"); <=> int status=system("clear"); --> clrscr(); for Unix */
}

/* Duplica parte de uma string */
char *duplicaString(char *str, int ini, int fim)
{
	int i = 0;
	char *res = (char *)malloc(sizeof(char) * (fim - ini));
	
	if (res == NULL) /* Erro malloc */
	{
		perror("duplicaString malloc");
		_exit(EXIT_FAILURE);
	}
	for (i = 0; ini < fim; i++, ini++) /* Copia string */
		res[i] = str[ini];
	res[i] = '\0'; 
	
	return res;
}

/* Junta 2 strings numa nova separando-as com espaço */
char *juntaString(char *str1, char *str2)
{
	int tam1 = strlen(str1), tam2 = strlen(str2), i, j;
	char *res = (char*)malloc(sizeof(char) * (tam1 + tam2 + 1));
	
	if (res == NULL) /* Erro malloc */
	{
		perror("juntaString malloc");
		_exit(EXIT_FAILURE);
	}
	for (i = 0; i < tam1; i++) /* Copia a 1ª string */
		res[i] = str1[i];
	res[i++] = ' '; /* Põe espaço */
	for (j = 0; j < tam2; j++, i++) /* Copia a 2ª string */
		res[i] = str2[j];
	
	return res;
}

/* Conta nº de vez que caracter aparece numa string */
int contaCaracter(char *str, char caracter)
{
	int i = 0, res = 0;
	
	for (i = 0; str[i] != '\0'; i++)
		if (str[i] == caracter) /* Se encontrar caracter */
			res++; /* Conta os caracteres pretendidos */
	
	return res;
}

/* Retira as plicas ou aspas de uma string */
char *tiraPlicasAspas(char *str)
{
	char *res = NULL, *aux = strdup(str);
	int pli = contaCaracter(str, '\''), asp = contaCaracter(str, '\"');
	
	if (pli > 1) /* Se tem 2 plicas */
	{
		if (str[0] == '\'') /* Se começa por plica */
			res = strtok(aux, "\'");
		else /* Se não começa por plica */
		{
			res = strtok(aux, "\'");
			res = strtok(NULL, "\'");
		}
	}
	else /* Se não tem 2 plicas */
	{
		if (asp > 1) /* Se tem 2 aspas */
		{
			if (str[0] == '\"') /* Se começa por aspas */
				res = strtok(aux, "\"");
			else /* Se não começa por aspas */
			{
				res = strtok(aux, "\"");
				res = strtok(NULL, "\"");
			}
		}
		else /* Se não tem aspas */
		{
			res = str; /* Retorna a string inalterada */
			free(aux); /* Liberta aux */
		}
	}
	
	return res;
}

/*
int criarMatriz(int ***a, int l, int c)
{
	int il = 0, ic = 0;
	
	if (l <= 0 || c <= 0) 
	{
		perror("criarMatriz l ou c <= 0");
		_exit(EXIT_FAILURE);
	}
	*a = (int **)malloc(sizeof(int *) * l);
	if (*a == NULL) 
	{
		perror("criarMatriz malloc");
		_exit(EXIT_FAILURE);
	}
	for (il = 0; il < l; il++)
	{
		(*a)[il] = (int *)malloc(sizeof(int) * c);
		if ((*a)[il] == NULL) 
		{
			perror("criarMatriz malloc 2");
			_exit(EXIT_FAILURE);
		}
		for (ic = 0; ic < c; ic++)
			(*a)[il][ic] = 0;
	}
	
	
	return l*c;
}
*/
/* lê 1 linha e retorna nº de caracteres lidos */
ssize_t readLine(int fildes, void *buf, size_t nbyte)
{
	char c = '\0', *bufChar = (char*)buf;
	int res = 0, n = 0, bytes = nbyte;
	
	while ((n = read(fildes, &c, 1) > 0) && (res < bytes) && (c != '\n') && (c != '\0'))
		bufChar[res++] = c;
	if (res == 0 && c == '\n') /* Se for apenas 1 Enter */
		res++;
	bufChar[res] = '\0';
	if (n < 0)
		_exit(1);
	
	return res;
}


Command readCommand(char *command)
{
	int i = 0, tam = 1024, j = 0, pid = 0, status = 0, pd[2], erro = 0;
	char **output = (char **)malloc(sizeof(char *) * tam), **aux;
	Command res = (Command)malloc(sizeof(NCommand));
	
	if (output == NULL || res == NULL)
	{
		perror("readCommand malloc");
		_exit(EXIT_FAILURE);
	}
	for (i = 0; i < tam; i++)
	{
		output[i] = (char *)malloc(sizeof(char) * TAM_LINHA);
		if (output[i] == NULL)
		{
			perror("readCommand malloc 2 output");
			_exit(EXIT_FAILURE);
		}
	}
	erro = pipe(pd);
	if (erro < 0)
	{
		perror("readCommand pipe");
		_exit(EXIT_FAILURE);
	}
	pid = fork();
	switch (pid)
	{
		case -1:
			perror("readCommand fork");
			_exit(EXIT_FAILURE);
		
		case 0: 
			close(pd[0]); 
			dup2(pd[1], 1); 
			close(pd[1]); 
			execl("/bin/sh", "sh", "-c", command, (char *) NULL);
			perror("readCommand execl");
			_exit(EXIT_FAILURE);
		
		default:
			close(pd[1]); 
			erro = waitpid(pid, &status, 0); 
			if (erro < 0) 
			{
				perror("readCommand waitpid");
				_exit(EXIT_FAILURE);
			}
			pid = WIFEXITED(status);
			if (WIFEXITED(status) > 0)
			{
				pid = WEXITSTATUS(status);
				if (pid == EXIT_FAILURE)
				{
					perror("readCommand WEXITSTATUS pid");
					_exit(EXIT_FAILURE);
				}
			}
	}
	i = 0;
	while (readLine(pd[0], output[i], TAM_LINHA) > 0)
	{
		i++;
		if (i == tam)
		{
			tam *= 2;
			aux = (char **)realloc(output, sizeof(char *) * tam);
			if (aux == NULL)
			{
				perror("readCommand realloc");
				_exit(EXIT_FAILURE);
			}
			output = aux;
			for (j = i; j < tam; j++) 
				output[j] = (char *)malloc(sizeof(char) * TAM_LINHA);
		}
	}
	close(pd[0]); 
	tam = i;
	res->output = output;
	res->lines = tam;
	
	return res;
}
