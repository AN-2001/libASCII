CC := gcc
lib := ascii
include := include
src := src
flags := -Wall -Werror -pedantic-errors -I$(include) -I. -g
output := libascii.a
include_path := /usr/include
lib_path := /usr/local/lib
obj := grid.o vector.o color.o ray.o triangle.o utill.o mat2x3.o

all: $(obj)
	ar rcs $(output) $^

install: 
	if [ ! -d $(include_path)/$(lib) ]; then cp -r $(include) $(include_path)/$(lib); else cp $(include)/* $(include_path)/$(lib); fi
	cp $(output) $(lib_path)

%.o: $(src)/%.c $(include)/%.h
	$(CC) -c $< $(flags) $(deps) -o $@  

clean:
	rm $(obj) $(output)
