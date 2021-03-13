#include <ascii/grid.h>
#include <stdlib.h>
#include <complex.h>
#include <stdio.h>
#include <math.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

#define INF 64
#define MAX_ITER 2e3 
#define WIDTH 128 
#define HEIGHT 64 

Position point;
Position len;
int fd;
double ang = 2.12165456165498;
complex double mandlebrot(complex double val, complex double offset);

int m1 = 0, m2 = 0;
void setup(){
	len = vectorCreate(1, 1);
	double r = 0.25;	
	point = vectorCreate(r * cos(ang) - 1, r * sin(ang));
    const char *pDevice = "/dev/input/mouse0";
    fd = open(pDevice, O_RDONLY | O_NONBLOCK);

    if(fd == -1){
        printf("ERROR Opening %s\n", pDevice);
		exit(1);
    }
}
complex double mandlebrot(complex double val, complex double offset){
	return val * val + offset;
}
Color gen(Position pos){
	double x = point.x + (double) ((pos.x / WIDTH) - 0.5) * len.x;
	double y = point.y + (double) ((pos.y / HEIGHT) - 0.5) * len.y;
	
	complex double v = mandlebrot(0, x + y * I);
	int cur;
	for(cur = 0; cur < MAX_ITER; cur++){
		if(cabs(v) > INF)
			break;
		v = mandlebrot(v, x + y * I);
	}
	int vl = (cur < MAX_ITER ? 255 : 0);
	return colorHSVToRGB(cur, 255, vl);
}
void update(Frame frame){
	struct input_event ie;
	int bytes = read(fd, &ie, sizeof(struct input_event));

	Direction mouse = vectorCreate(0, 0);
	if(bytes > 0){
		unsigned char *ptr = (unsigned char*)&ie;
		mouse.x = ((char)ptr[1]);
		mouse.y = ((char)ptr[2]);
		m1 = (ptr[0] & 0x1) > 0;
		m2 = (ptr[0] & 0x2) > 0;
	}
	if(m1){
		len.x *= 0.97;
		len.y *= 0.97;
	}
	if(m2){
		len.x *= 1.2;
		len.y *= 1.2;
	}
	point.x += mouse.x * 0.01f * len.x * 0.1f;
	point.y -= mouse.y * 0.01f * len.y * 0.1f;
	//gridClear(colorCreate(0, 0, 0));	
}
int main(int argc, char *argv[]){
	if(argc == 3)
		point = vectorCreate(strtod(argv[1], NULL), strtod(argv[2], NULL));
	gridOpen(WIDTH, HEIGHT, ASCII_FONT_TERM, setup, update, gen);
	gridSetMaxFrame(-1);
	gridDraw(NULL);
	gridClose();
	return 0;
}
