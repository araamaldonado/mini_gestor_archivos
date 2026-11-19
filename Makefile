CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

shell_test: $(wildcard src/*.c)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f shell_test