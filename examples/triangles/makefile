CC := gcc
libs := -L../../ -lascii -lm  
flags := -Wall -Werror -pedantic-errors 

triangles: main.c
	$(CC) main.c $(flags) $(libs) -o $@

clean:
	rm triangles
