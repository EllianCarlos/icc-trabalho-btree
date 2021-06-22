run:
	main.o

all:
	gcc main.c -o main.o

clean:
	rm *.o