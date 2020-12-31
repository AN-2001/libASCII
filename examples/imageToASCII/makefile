CC := gcc
exec := imageToASCII
flags := -Wall -Werror -pedantic-errors
libs := -lgd -lraster

all: imageToASCII.c
	$(CC) $^ $(flags) $(libs) -o $(exec)



clean:
	rm *.o *.a $(exec)
