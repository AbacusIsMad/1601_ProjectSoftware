all : *.c
	gcc *.c -o main `sdl2-config --cflags --libs` -lm

run : all
	./main
