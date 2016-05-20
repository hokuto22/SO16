#ifndef CLIENT_H
#define CLIENT_H

static int pid;
static int fifo;
static char *uname;

int vUsername(char *username);

void wvUsername(char **pedido);

char* sendMessage(int id, char **info, int in, int num);

void shell();

int main(void);

#endif
