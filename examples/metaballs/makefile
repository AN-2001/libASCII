CC := gcc
flags := --pedantic-errors -Wall -Werror -DASCII_USE_GD
libs := -lascii -lm -lgd

metaballs: metaballs.c
	$(CC) metaballs.c -o metaballs $(flags) $(libs)


clean:
	rm metaballs
