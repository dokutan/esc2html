# paths
BIN_DIR = /usr/bin
MAN_DIR = /usr/share/man/man1

# compiler options
CC = c++
CC_OPTIONS = -Wall -Wextra -O2 -std=c++20

build:
	$(CC) src/esc2html.cpp -o esc2html $(CC_OPTIONS)

clean:
	rm esc2html

install:
	cp esc2html $(BIN_DIR)
	cp esc2html.1 $(MAN_DIR)

uninstall:
	rm $(BIN_DIR)/esc2html
	rm $(MAN_DIR)/esc2html.1

