#include "outputProgress.h"

#include <stdlib.h>
#include <stdio.h>
#include "utill.h"
#include <unistd.h>
#include <string.h>
#include "color.h"

#define MAX_CHARS 60
#define PADDING 70
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
	char* output = malloc(MAX_CHARS + PADDING); 
	char* cpy = output;
	char c = FULL;
	output += sprintf(output, "%s", ASCII_TEXT_COLOR(255, 255, 0));
	for(int i = 0; i < MAX_CHARS;i++){
		if(i == chars){
			c = EMPTY;
			output += sprintf(output, "%s", ASCII_TEXT_COLOR(255, 0, 0));
		}

		output += sprintf(output, "%c",c); 
	}

	sprintf(output, "%s| %d%%\n", ASCII_TEXT_COLOR(0, 255, 0),(int)(precentage * 100));
	return cpy;
}

void writeOutProgress(int current, int max){

	char* progress = generateBar(current, max);

	if(current % LOADING_SIZE ==  0){
		loadingCtr++;
		loadingCtr = loadingCtr % LOADING_SIZE;
	}

	write(fileno(stdout), ASCII_TEXT_COLOR(0, 255, 0) "working:", 8 + strlen(ASCII_TEXT_COLOR(0, 255, 0)));
	write(fileno(stdout), loading + loadingCtr, 1);
	write(fileno(stdout), "\n", 1);
	write(fileno(stdout), progress, strlen(progress));  
	write(fileno(stdout), ASCII_MOVE_CURSOR(1, 1), ASCII_RESET_CURSOR_SIZE);

	free(progress);	
}

