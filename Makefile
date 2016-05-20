
CC = gcc
CFLAGS = -pedantic -Wall -Wextra -O2 -fmax-errors=5
SERVIDOR = server
CLIENTE = cliente

export DIRSO = $HOME

all: servidor cliente pastas

servidor: $(SERVIDOR).o utils.o treeA.o
	$(CC) $(CFLAGS) -o $(SERVIDOR) $(SERVIDOR).o utils.o treeA.o 

cliente: $(CLIENTE).o utils.o treeA.o
	$(CC) $(CFLAGS) -o $(CLIENTE) $(CLIENTE).o  utils.o treeA.o 

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

treeA.o: treeA.c treeA.h
	$(CC) $(CFLAGS) -c treeA.c

servidor.o: $(SERVIDOR).c utils.h
	$(CC) $(CFLAGS) -c $(SERVIDOR).c
	
	
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
