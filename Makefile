CC=gcc
CFLAGS=-ansi -pedantic -Wall -Wextra

.PHONY: clean

teste: main.o cn_integer.o
	@echo Compiling...
	@$(CC) -o teste main.o cn_integer.o $(CFLAGS)
	@echo Done!

main.o: main.c
	@$(CC) -c main.c

cn_integer.o: cn_integer.c
	@$(CC) -c cn_integer.c

clean:
	@rm -f *.o
	@rm -f teste
	@echo Cleaned!

