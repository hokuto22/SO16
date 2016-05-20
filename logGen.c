#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {

	int i, len ,fd= 0;

	char dummy[] = "DUMMY;coisas\n";
	char* s = NULL;

	fd = open("logGen.log",O_CREAT | O_RDWR);

	len = strlen(dummy);

	for(i=0; i<10; i++){
		write(fd,dummy,len);
		dummy[0]++;
	}

	s=strdup("Claudio;jokas\n");
	write(fd,s,strlen(s));

	for(i=0; i<20; i++){
		write(fd,dummy,len);
		dummy[1]++;
	}	
	return 0;
}
