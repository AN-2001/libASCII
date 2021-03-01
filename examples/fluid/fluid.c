#include <ascii/grid.h>
#include <math.h>
#include <ascii/utill.h>
#include <gd.h>

#define W 5
#define H 8
#define FR 0.6f
#define SR 0.01f
#define EPSILON (1e-8f)

#define ASPECT_W 16
#define ASPECT_H 9
#define RATIO 80
#define WIDTH (RATIO * ASPECT_W)
#define HEIGHT (RATIO * ASPECT_H)
#define SCALED_W (WIDTH / W)
#define SCALED_H (HEIGHT / H)

double board[SCALED_W][SCALED_H];
double copy[SCALED_W][SCALED_H];


Color generate(Position pos, Frame frame){
	int x = pos.x / W;
	int y = pos.y / H;
	double c = board[x][y];
	srand(pos.x + pos.y);
	if(c < EPSILON)
		return colorCreate(0, 0, 0);
	Color out = colorCreate(20, 20, 120);
	Color offset = colorCreate(rand() % 255, rand() % 255, rand() % 255);
	c = maxf(c, 0.5f);
	return	colorAdd(out, colorMult(offset, c));  
}

void sim(){

	for(int y = 0; y < SCALED_H; y++){
		for(int x = 0; x < SCALED_W; x++){
			if(board[x][y] <= EPSILON)
				continue;

			double fallRate = board[x][y] * FR;
			double flowRate = board[x][y] * SR;

			if( board[x][y+1] < 1.0f - EPSILON && y < SCALED_H - 1){
				double b4 = copy[x][y];
				copy[x][y] = maxf(board[x][y] - fallRate, 0);
				copy[x][y+1] += b4 - copy[x][y]; 
			} else if( x > 1 && x < SCALED_W - 1){
				double alteredFlowRate = board[x][y] - maxf(board[x][y] - flowRate, 0);
				double r = maxf(1.0f - board[x+1][y], 0);
				double l = maxf(1.0f - board[x-1][y], 0);
				if(r+l == 0)
					continue;
				double rightRatio = r / (r + l); 
				double leftRatio = l / (r + l); 

				double rrate = alteredFlowRate * (rightRatio);
				double lrate = alteredFlowRate * (leftRatio);

				copy[x-1][y] += lrate; 
				copy[x+1][y] += rrate; 
				copy[x][y] -= alteredFlowRate;
			}

		}
	}

}

void cpyBoard(double dest[][SCALED_H], double src[][SCALED_H]){
	for(int i = 0; i < SCALED_W;i++){
		for(int j = 0; j < SCALED_H; j++){
			dest[i][j] = src[i][j];
		}
	}
}
void update(Frame frame){
	cpyBoard(copy, board);
	if(frame > 50)
		sim();
	cpyBoard(board, copy);
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
			if(r + g + b > 0)
				board[i][j] = 1.0f;
			else
				board[i][j] = 0.0f;
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

	gridOpen_(WIDTH, HEIGHT, NULL, update, generate);
	gridSetMaxFrame(300);
	gridDraw(NULL);
	gridClose();

	return 0;
}
