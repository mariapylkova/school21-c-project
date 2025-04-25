CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11

all: s21_grep test

s21_grep: s21_grep.c
	$(CC) $(CFLAGS) -o s21_grep s21_grep.c

clean:
	rm -f s21_grep

test: test.sh
	chmod +x test.sh
	./test.sh

rebuild: clean all
	chmod +x test.sh
