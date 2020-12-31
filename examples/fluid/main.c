#include <ascii/grid.h>
#include <ascii/utill.h>
#include <gd.h>

#define W 5
#define H 8
#define FR 0.6
#define SR 0.9
#define EPSILON 1e-9
#define ASPECT_W 16
#define ASPECT_H 9
#define RATIO 80
#define WIDTH (RATIO * ASPECT_W)
#define HEIGHT (RATIO * ASPECT_H)
#define SCALED_W (WIDTH / W)
#define SCALED_H (HEIGHT / H)

double board[SCALED_W][SCALED_H];
double copy[SCALED_W][SCALED_H];


Color generate(Position pos, Dimention dim, Frame_t frame){
	int x = pos.x / W;
	int y = pos.y / H;
	if(frame < 25){
		if(board[x][y] > EPSILON){
			Color out = colorCreate(40, 100, 255);
			return out;
		}else{
			return colorCreate(0, 0, 0);
		}
	}
	double fallRate = board[x][y] * FR;
	double flowRate = board[x][y] * SR;
	if(board[x][y] > EPSILON){
		if(board[x][y+1] < 1.0 - EPSILON && y < SCALED_H - 2  ){
			double before = copy[x][y];
			copy[x][y] = maxf(copy[x][y] - fallRate, EPSILON);
			copy[x][y+1] += before - copy[x][y];

		}else if( x > 1 && x < SCALED_W - 1){
			
			double alteredFlowRate = copy[x][y] - maxf(copy[x][y] - flowRate, EPSILON);
			double rrate = alteredFlowRate * 0.5f;
			double lrate = alteredFlowRate * 0.5f;

			copy[x][y] = copy[x][y] - rrate - lrate;

			copy[x+1][y] += rrate; 
			copy[x-1][y] += lrate; 

		}
	}else{
		board[x][y] = 0;
	}

	double c = board[x][y];
	if(c > 0){
		double fixed = minf(maxf(c, 0.2), 1);
		Color offset = colorCreate(0, 60, 70);
		Color out = colorCreate(30, 120, 255);
		return colorAdd(offset, colorMult(out, fixed));
	}else{
		return colorCreate(0, 0, 0);
	}
}

void setup(){

	for(int i = 0; i < SCALED_W;i++){
		for(int j = 0; j < SCALED_H; j++){
			copy[i][j] = board[i][j];
		}
	}
	
}

void update(PixelGrid grid, Frame_t frame){
	gridClear(grid);
	for(int i = 0; i < SCALED_W;i++){
		for(int j = 0; j < SCALED_H; j++){
			board[i][j] = copy[i][j];
		}
	}
	setup();
}

int readImage(const char* path){
	FILE* img = fopen(path, "rb");
	gdImagePtr image = gdImageCreateFromPng(img);	
	if(image == NULL){
		fclose(img);
		return 1;
	}
	fclose(img);
	if(image->sx != WIDTH || image->sy != HEIGHT){
		fprintf(stderr, "image must be %dx%d\n", WIDTH, HEIGHT);
		return 1;
	}
	for(int i = 0; i < SCALED_W;i++){
		for(int j = 0; j < SCALED_H;j++){
			int c = gdImageGetTrueColorPixel(image, i * W, j * H);
			int r = gdTrueColorGetRed(c);	
			int g = gdTrueColorGetGreen(c);
			int b = gdTrueColorGetBlue(c);
			if(r + g + b > 0){
				board[i][j] = 5;
			}
		}
	}
	return 0;
}
int main(int argc, const char* argv[]){
	if(argc == 1){
		fprintf(stderr, "please enter an image path\n");
		return 1;
	}

	if(readImage(argv[1])){
		return 1;
	}

	PixelGrid grid = gridCreate(WIDTH, HEIGHT,setup, update, generate);
	gridSetMaxFrame(grid, 600);

	gridDrawToImage(grid, "output/out");
	gridFree(grid);	

	return 0;
}
