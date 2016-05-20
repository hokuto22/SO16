#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void geraHuge() {
	int fd = open("Dummy/huge.txt",O_CREAT | O_RDWR);
	int i,j;	
	for(i=0;i<1000;i++) {
		for(j=0;j<100;j++)
			write(fd,"a",1);
	}
	close(fd);
}

void geraTxt() {
	int fd = open("Dummy/1.txt",O_CREAT | O_RDWR);
	
	write(fd,"lixo    lixo",12);
	
	close(fd);
}

void geraPdf() {
	int fd = open("Dummy/2.pdf",O_CREAT | O_RDWR);

	write(fd,"ousg",4);

	close(fd);
}

void geraFanecas() {
	int fd = open("Dummy/4.fanecas",O_CREAT | O_RDWR);

	write(fd,"If you reach here\nYou ROCK!",27);

	close(fd);
}

int main() {

	geraHuge(); // huge.txt
	geraTxt(); // 1.txt
	geraPdf(); // 2.pdf
	geraFanecas(); // 4.fanecas

	return 0;
}
