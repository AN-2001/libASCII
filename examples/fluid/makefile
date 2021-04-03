CC := gcc
flags := --pedantic-errors -Wall -Werror -DASCII_USE_GD
libs := -lascii -lm -lgd -ludev -lpthread

fluid : fluid.c
	$(CC) fluid.c -o fluid $(flags) $(libs)

clean :
	rm fluid
