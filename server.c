#include "utils.h"
#include "treeA.h" /* ALTERAR HEADER */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <sys/wait.h>

#define US_LOGIN_SUCCESSFUL	-1
#define US_NOT_REGISTERED   -2
#define US_ALREADY_EXISTS   -3
#define US_WRONG_PASS  		-4

static tree *users;
static char path[] = "/.Backup/";
static char pathAcc[] = "/.Backup/Accounts/";

void loaddata(){

	int fd = 0, error = 0, i = 0;
	char line[1024];
	char *dir, *token, *strArray[128];
 	
 	users = createTree();

	dir = strdup(pathAcc);
	strcat(dir, "users.log");

	fd = open(dir, O_RDONLY);

	if(fd<0){
		open(dir, O_RDWR | O_TRUNC);
	}
	else{

	while((readLine(fd,line, sizeof(line))) > 0){
	
		token = strtok(line, ":");

		while(token != NULL){
			strArray[i] = strdup(token);
			token = strtok(NULL, ":");
			i++;
		}

		insertTree(users, strArray[0], strArray[1]);
	}

	error = close(fd);
	if (error < 0)
	{
		perror("loaddata close");
		_exit(EXIT_FAILURE);
	}
	}
}

void sendMessage(char *fifo, char *message)
{
	int fd = 0, n = 0;
	char *ff = NULL;
	const char *env = getenv("HOME");

	ff=strdup(env);
	strcat(ff, "/.Backup/FIFOs/fifo");
	printf("\n--%s--\n",ff);
	fd = open(ff, O_WRONLY);
	if(fd < 0){
		perror("sendMessage open");
		_exit(EXIT_FAILURE);
	}

	n = write(1, message, strlen(message));
	if(n<0){
		perror("sendMessage write");
		_exit(EXIT_FAILURE);
	}

	close(fd);
}

int backup(char *ppid, char *username, char *files){

	int n = 0, i = 0, m = 0, j = 0, k = 0, l = 0, pid = 0; 
	char *token, *strArray[1024], *pArray[1024], *sArray[1024], *f, *ddir, *mdir, *dir, *dir2, *mkd, *filen, *s1, *sha;
	Command c = NULL;

	pid = atoi(ppid);
	dir2 = strdup(pathAcc);
	strcat(dir2, "/");
	strcat(dir2, username);
	token =  strtok( files, " ");

	while(token != NULL){
		strArray[i] = strdup(token);
		token =  strtok(NULL, " ");
		i++;
	}

	for(k = 0; k < i; k++){
		
		dir = strdup(strArray[k]);
		
		token =  strtok( files, "/");

		while(token != NULL){
			pArray[n] = strdup(token);
			token =  strtok(NULL, " ");
			n++;
		}

		filen = pArray[n-1];
		f = strdup("ls");
		strcat(f, dir);
		c = readCommand(f);

		for(l = 0; l <c->lines; l++){

			sha = strdup("sha1sum ");
			strcat(sha, c->output[l]);

			c = readCommand(sha);

			token = strtok( c->output[0], " ");

			while(token != NULL){
				sArray[m] = strdup(token);
				token = strtok(NULL, " ");
				m++;
			}

			s1 = sArray[0];


			for( j = 0; j < n-2; j++){
				strcat(dir2, pArray[j]);
				mkd = strdup("mkdir ");
				strcat(mkd, dir2);
				c = readCommand(mkd);

				if(j == n-2){
					if(c->lines == 1 ){
						perror("backup mkdir");
						_exit(EXIT_FAILURE);
					}
				}
			}
			ddir = strdup(mkd);
			strcat(ddir, "/data/");
			c = readCommand(ddir);
			if(c->lines == 1 ){
				perror("backup mkdir");
				_exit(EXIT_FAILURE);
			}
			mdir = strdup(mkd);
			strcat(mdir, "/metadata/");
			c = readCommand(mdir);
			if(c->lines == 1 ){
				perror("backup mkdir");
				_exit(EXIT_FAILURE);
			}
			strcpy(mkd, "gzip -c ");
			strcat(mkd, dir);
			strcat(mkd, "> ");
			strcat(mkd, ddir );
			strcat(mkd, s1);

			c = readCommand(mkd);
			strcpy(mkd, "ln -s -f ");
			strcat(mkd, ddir);
			strcat(mkd, s1);
			strcat(mkd, " ");
			strcat(mkd, mdir);
			strcat(mkd, filen);

			c = readCommand(mkd);
			kill(pid, SIGUSR1);
		}
	}
	return 0; /*CORREU BEM!*/
}

/* Armazena os dados dos utilizadores em ficheiro: 
retorna 0 em caso de sucesso. */
int saveUsers(node *users)
{
	FILE *fp;

	int error = 0;
	char *dir;

	dir = strdup(pathAcc);
	strcat(dir, "users.log");
	fp = fopen(dir,"w+");
	if (fp == NULL )
	{
		perror("saveUsers fopen");
		_exit(EXIT_FAILURE);
	}

	printTree(users, fp);

	error = fclose(fp);
	if (error < 0)
	{
		perror("saveUsers write 2");
		_exit(EXIT_FAILURE);
	}
	
	return 0;
}

int restore(char *ppid, char *username, char *files){

	int n = 0, j = 0, pid = 0; 
	char *token, *pArray[1024], *dir, *dir2, *filen, *rest, *find, *drest;
	Command c = NULL;

	pid = atoi(ppid);
	dir2 = strdup(path);
	strcat(dir2, username);
	strcat(dir2, "/");

	dir = strdup(files);
		
	token = strtok( files, "/");

	while(token != NULL){
		pArray[n] = strdup(token);
		token =  strtok(NULL, "/");
		n++;
	}

	filen = pArray[n-1];
	drest = strdup(pArray[j]);
	for( j = 1; j < n-2; j++ ){
		strcat(drest,pArray[j]);
		strcat(drest,"/");
	}

	strcat(drest, "metadata/");
	strcat(drest, filen);
	find = strdup("readlink -e ");
	strcat(find, drest);

	c = readCommand(find);

	rest = strdup("gunzip -c ");
	strcat(rest, c->output[0]);
	strcat(rest, " > ");
	strcat(rest, dir);

	c = readCommand(rest);
	if(c->lines == 0) 
		kill(pid,SIGUSR2);

	return 0;
}

int login( char *username, char *pass){
	int res = 0;

	res = existUser(users->root,username,pass);
	if (res == -2) 
		return US_NOT_REGISTERED;
	else{ 
		if(res == -4) 
			return US_WRONG_PASS;
	} 
		return res;
}

int reg(char *username, char *pass){
	int res;
	char dir[1024];
	Command c = NULL;

	res = existUser(users->root,username,pass);
	printf("\n\n%d\n\n",res);
	if(res == -2){

// 	strcpy(dir, "mkdir ");
// 	strcat(dir, pathAcc);
// 	strcat(dir, username);
// //	strcat(dir, FOLDER_NAME);

//     c = readCommand(dir);

//     if(c->lines > 0){
//     	perror("reg mkdir");
//     	_exit(EXIT_FAILURE);	
//     } 

	insertTree(users, username, pass);

	res = 0;
	}
return res;
}

int readFifo(char *fifo, char ***message)
{
	int fd = 0, i = 0, n = 0, erro = 0;
	char buffer[1024];
	char *token, *strArray[128];
	
	printf("\n%s\n",fifo);

	fd = open(fifo, O_RDONLY);
	if (fd < 0)
	{
		perror("readFifo open");
		_exit(EXIT_FAILURE);
	}
	
	n = readLine(fd, buffer, sizeof(buffer));
	
	if (n < 0){
		perror("shell read");
		//_exit(EXIT_FAILURE);
	}
	printf("\nLI! -- %s\n", buffer);

	token = strtok(buffer,":");

	while(token != NULL){
		strArray[i] = strdup(token);
		token = strtok(NULL, ":");
		i++;
	}

	erro = close(fd);
	if (erro < 0)
	{
		perror("readFifo close");
		_exit(EXIT_FAILURE);
	}
	*message = strArray;
	
	/*printf("MENSAGEM RECEBIDA = [%s]", tipo);
	for (j = 0; j < i; j++)
		printf(":[%s]", buffer[j]);
	printf("\n");*/

	while(i>0)
		printf("\n%s\n",strArray[i--]);

	if (strcmp(strArray[1], "log") == 0) return 1;
	if (strcmp(strArray[1], "reg") == 0) return 2;
	if (strcmp(strArray[2], "backup") == 0) return 3;
	if (strcmp(strArray[2], "restore") == 0) return 4;
	
	return -1; /* Tipo de mensagem inválido */
}

int main(){
	int fd = 0, tipe = 0, success = 0, pid = 0; 
	char **message = NULL; 
	char *dir = NULL;
	const char *env = getenv("HOME");
	pid_t cpid[5];
	int contaF = 0;

	loaddata();

	dir = strdup(env);
	strcat(dir,path);
	strcat(dir,"FIFOs/fifo");
	if (fd < 0){
		if(errno != EEXIST){
			perror("main mkfifo");
			_exit(EXIT_FAILURE);
		}
	}

	fd = open(dir, O_RDONLY);
	if(fd < 0){
		perror("main open fifo");
		_exit(EXIT_FAILURE);
	}

	while(1){
		dir = strdup(env);
		strcat(dir,path);
		strcat(dir,"FIFOs/fifo");
		message = NULL;
		printf("\n|%s|\n",dir);
		tipe = readFifo(dir, &message);
		if(tipe==1 || tipe == 2){
				if(tipe == 1) {
					printf("\nM3- %s\nM4- %s\n",message[3],message[4]);
				success = login(message[3],message[4]);
					switch(success){
						case 1:
							sendMessage(message[3], "US_LOGIN_SUCCESSFUL");
							break;
						case US_NOT_REGISTERED: /* Utilizador não existe */
			    			sendMessage(message[3], "UT_NAO_EXISTE");
			    			break;
			    			
		    			case US_WRONG_PASS: /* Pass errada */
		    				sendMessage(message[3], "UT_PASS_ERRADA");
		    				break;
		    			
			    		default:
			    			sendMessage(message[3], "ERRO");
			    			break;
		    		}
    			}

 				else if(tipe == 2){
		    	success = reg(message[3], message[4]);
		    		switch (success)
		    		{
		    			case 0: /* Sucesso */
				    		sendMessage(message[3], "OK");
		    				break;
		    			
		    			case US_ALREADY_EXISTS: /* Utilizador já existe */
			    			sendMessage(message[3], "US_ALREADY_EXISTS");
			    			break;
		    			
			    		default:
		    				sendMessage(message[3], "ERRO");
		    				break;
		    		}
    			}
    		}
    	else if(tipe > 2){
    		if(contaF < 5){
    			cpid[contaF] = fork();
    			
    			if(tipe == 3 && cpid[contaF] == 0){
    				success = backup(message[3],message[0],message[4]);
    				pid = atoi(message[3]);
    				switch(success)
   					{
   						case 0:
	    					kill(pid, SIGCONT);
   							break;
	   	    			default:
	   	    				kill(pid, SIGTERM);
	    			}
	    			_exit(0);
	   			}

				else if(tipe == 4 && cpid[contaF] == 0){
					success = restore(message[3], message[0],message[4]);
					pid = atoi(message[3]);
					switch(success){
						case 0:
	    					kill(pid, SIGCONT);
   							break;
	   	    			default:
	    					kill(pid, SIGTERM);
					}
					_exit(0); /*restore*/    		
				}
				contaF++;
			}
		}
		
		wait(NULL);

	}

	return 0;	
}
