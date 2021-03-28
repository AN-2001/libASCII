CC := gcc
flags := --pedantic-errors -Wall -Werror -DASCII_USE_GD
libs := -lascii -lm -lgd -ludev -lpthread


mandlebrot: mandlebrot.c
	$(CC) mandlebrot.c -o mandlebrot $(flags) $(libs)


clean:
	rm mandlebrot
