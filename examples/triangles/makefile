CC := gcc
libs := -L../../ -lascii -lm -lgd
flags := -Wall -Werror -pedantic-errors

triangles: main.c
	$(CC) main.c $(libs) -o $@

clean:
	rm triangles
