CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11

all: s21_cat test

s21_cat: s21_cat.c
	$(CC) $(CFLAGS) -o s21_cat s21_cat.c

clean:
	rm -f s21_cat

test: test.sh
	chmod +x test.sh
	bash ./test.sh

rebuild: clean all