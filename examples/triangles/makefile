CC := gcc
libs := -L../../ -lascii -lm -lgd
flags := -Wall -Werror -pedantic-errors  -DASCII_USE_GD

triangles: main.c
	$(CC) main.c $(flags) $(libs) -o $@

clean:
	rm triangles
