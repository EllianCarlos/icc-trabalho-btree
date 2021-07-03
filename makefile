run: all
	@./main.o

runWin: allWin
	@./main.exe

all:
	@gcc *.c *.h -o main.o

allWin:
	@gcc *.c *.h -o main.exe

clean:
	@rm *.o *.bin

zip:
	zip makefile.zip -r makefile *.c *.h README.md