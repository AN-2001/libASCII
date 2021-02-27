# libASCII
a **toy** library for drawing images but in ASCII!

![gif!](https://github.com/0Cufox0/libASCII/blob/main/docs/triangles_1.gif)
![gif!](https://github.com/0Cufox0/libASCII/blob/main/docs/fluid_1.gif)

## dependencies
- libgd: for outputting images, added an option to compile without it if you only want to output to terminals!

## how to install:
- run `make` to compile
- after compiling you can simply link your programs with libascii.a, alternatively **not recommended** you can run `make install` and link with it like any other library
## how to use
check out the examples!
each one can be compiled with `make` 
after running the program it'll write out a bunch of images to the output directory, to get your gif/video output run one of the scripts inside the scripts directory.

**to run the scripts you need ffmpeg!**

each script follows this syntax

`./script <path to file containing images> {path to output}`
