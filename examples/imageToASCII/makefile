CC := gcc
fags := -Wall -Werror -pedantic-errors -g
libs := -L../../ -lascii -lm -lgd

imageToASCII: imageToASCII.c
	$(CC) imageToASCII.c $(flags) $(libs) -o $@

clean:
	rm imageToASCII
