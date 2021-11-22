#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int numAccs;
int main(int argc, char *argv[]){
	int numTrans=0;
	size_t len = 0;
	char *linebuf = NULL;
	FILE *file = fopen(argv[1], "r");

	getline(&linebuf, &len, file);
	numAccs = atoi(linebuf);
	
	int lines = 0;
	while(getline(&linebuf, &len, file)!= -1){
		lines++;
	}
	printf("total lines in file: %d\n", lines);
}
