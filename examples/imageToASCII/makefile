CC := gcc
flags := -Wall -Werror -pedantic-errors  -DASCII_USE_GD
libs := -L../../ -lascii -lm -lgd

imageToASCII: imageToASCII.c
	$(CC) imageToASCII.c $(flags) $(libs) -o $@

clean:
	rm imageToASCII
