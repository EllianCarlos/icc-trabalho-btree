run: all
	./main.o

all:
	gcc *.c -o main.o

clean:
	rm *.o