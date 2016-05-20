
CC = gcc
CFLAGS = -pedantic -Wall -Wextra -O2 -fmax-errors=5
SERVIDOR = server
CLIENTE = cliente
LIGARSERVIDOR = ligarServer

export DIRSO = $HOME

all: servidor cliente  ligarServidor pastas

servidor: $(SERVIDOR).o utils.o treeA.o
	$(CC) $(CFLAGS) -o $(SERVIDOR) $(SERVIDOR).o utils.o treeA.o 

ligarServidor: $(LIGARSERVIDOR).o utils.o
	$(CC) $(CFLAGS) -o $(LIGARSERVIDOR) $(LIGARSERVIDOR).o utils.o

cliente: $(CLIENTE).o utils.o treeA.o
	$(CC) $(CFLAGS) -o $(CLIENTE) $(CLIENTE).o  utils.o treeA.o 

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

treeA.o: treeA.c treeA.h
	$(CC) $(CFLAGS) -c treeA.c

servidor.o: $(SERVIDOR).c utils.h
	$(CC) $(CFLAGS) -c $(SERVIDOR).c
	
ligarServer.o: $(LIGARSERVIDOR).c utils.h
	$(CC) $(CFLAGS) -c $(LIGARSERVIDOR).c	
	
cliente.o: $(CLIENTE).c cliente.h utils.h  
	$(CC) $(CFLAGS) -c $(CLIENTE).c


clean: 
	clear
	rm -rf $(SERVIDOR) $(CLIENTE) *.o fifo FIFOs


clrscr: 
	clear
	
pastas: 
	mkdir -p Accounts
	mkdir -p FIFOs
	mkdir -p Dummies
	
kill: 
	clear
	killall -SIGKILL $(SERVIDOR)
