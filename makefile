
CC=clang
FLAGS=-std=c11 -Wall -Werror

.DEFAULT: tester

.PHONY: clean

tester: tester.c tree.o list.o common.h utils.o
	$(CC) $(FLAGS) $^ -o $@

tree.o: tree.c tree.h common.h list.o utils.o
	$(CC) $(FLAGS) $< -c

list.o: list.c list.h common.h utils.o
	$(CC) $(FLAGS) list.c -c

utils.o: utils.c utils.h
	$(CC) $(FLAGS) utils.c -c


run: tester
	./tester

clean:
	rm -f storage
	rm -f *.o
