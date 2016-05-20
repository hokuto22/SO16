#include "utils.h"
#include "treeA.h" /* ALTERAR HEADER */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>

#define US_LOGIN_SUCCESSFUL	-1
#define US_NOT_REGISTERED   -2
#define US_ALREADY_EXISTS   -3
#define US_WRONG_PASS  		-4

static tree *users;
static char path[] = "$DIRSO/.Backup/";
static char pathAcc[] = "$DIRSO/.Backup/Accounts/";
static int processesinqueue;

void loaddata(){

	int n = 0, fd = 0, error = 0, i = 0;
	char line[1024];
	char *dir, *token, *strArray[128];
 	
 	users = createTree();

	strcpy(dir, pathAcc);
	strcat(dir, "users.log");

	fd = open(dir, O_RDONLY);

	if(fd<0){
		perror("loaddata open");
		_exit(EXIT_FAILURE);
	}

	while((readline(fd,line, sizeof(line))) > 0){
	
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

void sendMessage(char *fifo, char *message)
{
	int i = 0, n = 0, response = 0, pid = 0;
	char send[512] = {0}, receve[512] = {0}; /* Tamanho máximo das mensagens 512 bytes */
	char *error = NULL, *res = NULL, ff[512], pidC[128];
	
	strcpy(ff, "FIFOs/");
	strcat(ff, fifo);
	fd = open(ff, O_WRONLY);
	if(fd < 0){
		perror("sendMessage open");
		_exit(EXIT_FAILURE);
	}

	n = write(fd, message, sizeof(char) * strlen(message));
	if(n<0){
		perror("sendMessage write");
		_exit(EXIT_FAILURE);
	}

	close(fd);
}

int backup(int ppid, char *username, char *files){

	int error = 0, n = 0, i = 0, m = 0, j = 0, k = 0, l = 0; 
	char *token, *strArray[1024], *pArray[1024], *sArray[1024], *f, *ddir, *mdir, *dir, *dir2, *mkd, *filen, *s1, *sha;
	Command c = NULL;

	strcpy(dir2, pathAcc);
	strcat(dir2, "/");
	strcat(dir2, username);
	token =  strtok( files, " ");

	while(token != NULL){
		strArray[i] = strdup(token);
		token =  strtok(NULL, " ");
		i++
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
		strcpy(f, "ls");
		strcat(f, dir);
		c = readCommand(f);

		for(l = 0; l <c->lines; l++){

			strcpy(sha, "sha1sum ");
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
				strcpy(mkd, "mkdir ");
				strcat(mkd, dir2);
				c = command(mkd);

				if(j == n-2){
					if(c->lines == 1 ){
						perror("backup mkdir");
						_exit(EXIT_FAILURE);
					}
				}
			}
			strcpy(ddir, mkd);
			strcat(ddir, "/data/");
			c = command(ddir);
			if(c->lines == 1 ){
				perror("backup mkdir");
				_exit(EXIT_FAILURE);
			}
			strcpy(mdir, mkd);
			strcat(mdir, "/metadata/");
			c = command(mdir);
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
			strcat(mkd, pArray[n-1]);

			c = readCommand(mkd);
			kill(ppid, SIGUSR1);
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
	char file[1024];
	char *dir;

	strcpy(dir, pathAcc);
	strcat(dir, "users.log");
	fp = fopen(dir,"w+");
	if (fp < 0)
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

int restore(int ppid, char *username, char *files){

	int n = 0, j = 0; 
	char *token, *pArray[1024], *dir, *dir2, *filen, *rest, *find, *drest;
	Command c = NULL;

	strcpy(dir2, path);
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

	for( j = 0; j < n-2; j++ ){
		strcat(drest,pArray[j]);
		strcat(drest,"/");
	}

	strcat(drest, "metadata/");
	strcat(drest, filen);
	strcat(find, "readlink -e ");
	strcat(find, drest);

	c = readCommand(find);

	strcpy(rest, "gunzip -c ");
	strcat(rest, c->output[0]);
	strcat(rest, " > ");
	strcat(rest, files);

	c = readCommand(rest);
	if(c->linhas == 0)
		kill(ppid,SIGUSR2);

	return 0;
}

int login(node *u, char *username, char *pass){
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

	if(res == -2){

	strcpy(dir, "mkdir ");
	strcat(dir, pathAcc);
	strcat(dir, username);
	strcat(dir, FOLDER_NAME);

    c = readCommand(dir);

    if(c->lines > 0){
    	perror("reg mkdir");
    	_exit(EXIT_FAILURE);	
    } 

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
	
	fd = open(fifo, O_RDONLY);
	if (fd < 0)
	{
		perror("readFifo open");
		_exit(EXIT_FAILURE);
	}
	
	n = readline(fd, buffer, sizeof(buffer));
	
	if (n < 0){
		perror("shell read");
		_exit(EXIT_FAILURE);
	}
	
	token = strtok(buffer, " ");

	while(token != NULL){
		strArray[i] = strdup(token);
		token = strtok(NULL, " ");
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
	
	if (strcmp(strArray[1], "log") == 0) return 1;
	if (strcmp(strArray[1], "reg") == 0) return 2;
	if (strcmp(strArray[2], "backup") == 0) return 3;
	if (strcmp(strArray[2], "restore") == 0) return 4;
	
	return -1; /* Tipo de mensagem inválido */
}

int main(){
	int fd = 0, tipe = 0, success = 0, pid = 0, autent = 0; 
	char *message = NULL; 
	char *dir;
	pid_t cpid[5];
	int contaF = 0, status;

	loaddata();

	strcpy(dir, path);
	strcat(dir,"FIFOs/fifo");
	fd =makefifo(dir, 0666);
	if (fd < 0){
		if(errno != EEXIST){
			perror("main mkfifo");
			_exit(EXIT_FAILURE);
		}
	}
	while(1){

		fd = open("$DIRSO/.Backup/FIFOs/fifo", O_RDONLY);
		if(fd < 0){
			perror("main open fifo");
			_exit(EXIT_FAILURE);
		}
		message = NULL;
		tipe = readFifo("$DIRSO/.Backup/FIFOs/fifo", &message);
		
		if(tipe==1 || tipe == 2){
				if(tipe == 1) {
				success = login(users->root,message[3],message[4]);
					switch(success){
						case 0:
							sendMessage(message[2], "US_LOGIN_SUCCESSFUL");
						case US_NOT_REGISTERED: /* Utilizador não existe */
			    			sendMessage(message[2], "UT_NAO_EXISTE");
			    			break;
			    			
		    			case US_WRONG_PASS: /* Pass errada */
		    				sendMessage(message[3], "UT_PASS_ERRADA");
		    				break;
		    			
			    		default:
			    			sendMessage(message[2], "ERRO");
			    			break;
		    		}
    				break;
    			}

 				else if(tipe == 2){
		    	success = reg(message[3], message[4]);
		    		switch (success)
		    		{
		    			case 0: /* Sucesso */
				    		sendMessage(message[2], "OK");
		    				break;
		    			
		    			case US_ALREADY_EXISTS: /* Utilizador já existe */
			    			sendMessage(message[2], "US_ALREADY_EXISTS");
			    			break;
		    			
			    		default:
		    				sendMessage(message[2], "ERRO");
		    				break;
		    		}
    				break;
    			}
    		}
    	else if(tipe > 2){
    		if(contaF < 5){
    			cpid[contaF] = fork();
    			contaF++;
    			if(tipe == 3 && cpid[contaF] == 0){
    				success = backup(pid,message[0],&message[3]);
    				switch(success)
   					{
   						case 0:
	    					sendMessage(message[0], "Successfull Backup!");
   							break;
	   	    			default:
	    					sendMessage(message[0], "Fail Backup!");
	    			}
	    			_exit(0);
	   			}

				else if(tipe == 4 && cpid[contaF] == 0){
					success = restore(pid, message[0],&message[3]);
					switch(success){
						case 0:
	    					sendMessage(message[0], "Successfull Restore!");
   							break;
	   	    			default:
	    					sendMessage(message[0], "Fail Restore!");
					}
					_exit(0); /*restore*/    		
				}
			}
		}
		
		wait(NULL);


		return EXIT_SUCCESS;
	}

	return 0;	
}
