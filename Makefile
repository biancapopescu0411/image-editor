# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# define targets
TARGETS=image_editor

build: $(TARGETS)

image_editor: *.c
	$(CC) $(CFLAGS) *.c -lm -o image_editor

pack:
	zip -FSr 325CA_MariaBianca_Popescu_Tema3.zip README Makefile *.c *.h

clean:
	rm -f $(TARGETS)

.PHONY: pack clean
