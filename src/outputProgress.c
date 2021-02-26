#include "outputProgress.h"

#include <stdlib.h>
#include <stdio.h>
#include "utill.h"
#include <unistd.h>

#define MAX_CHARS 60
#define FULL '='
#define EMPTY '-'
#define LOADING_SIZE 4
static	const char * const loading = "\\|/-";
static int loadingCtr = 0;
char* generateBar(int current, int max){
	if(current > max){
		return NULL;
	}
	double precentage = (double)current / (double)max;
	int chars = precentage  * (double)MAX_CHARS;
	char* output = malloc(MAX_CHARS + 10); 
	char* cpy = output;
	for(int i = 0; i < MAX_CHARS;i++){
		char c = (i < chars ? FULL : EMPTY);
		sprintf(output, "%c", c); 
		output++;
	}

	sprintf(output, "| %d%%\n", (int)(precentage * 100));
	return cpy;
}

void writeOutProgress(int current, int max){

	char* progress = generateBar(current, max);

	if(current % LOADING_SIZE ==  0){
		loadingCtr++;
		loadingCtr = loadingCtr % LOADING_SIZE;
	}

	printf("working:%c\n%s%s%s", loading[loadingCtr], progress,
								ASCII_CLEAR_SCREEN, ASCII_MOVE_CURSOR(1, 1) );
	free(progress);	
}

