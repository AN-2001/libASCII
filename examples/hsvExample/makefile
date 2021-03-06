CC := gcc
flags := -pedantic-errors -Wall -Werror -DASCII_USE_GD
libs := -lascii -lm -lgd


hsv : hsv.c
	$(CC) hsv.c -o $@ $(flags) $(libs)


clean:
	rm hsv
