run: all
	@./main.o

all:
	@gcc *.c *.h -o main.o

clean:
	@rm *.o