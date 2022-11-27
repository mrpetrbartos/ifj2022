CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror

CFILES=$(wildcard *.c)
HFILES=$(wildcard *.h)
OFILES=$(CFILES:.c=.o)
NAME=xbarto0g

all: ifj22

ifj22: $(OFILES)
	$(CC) $(CFLAGS) -g -o $(NAME) $^

%.o: %.c $(HFILES)
	$(CC) $(CFLAGS) -g -c $< -o $@

clean:
	rm -rf *.o
	rm -f ifj22
	rm -f $(NAME).zip

pack: $(CFILES) $(HFILES) Makefile rozdeleni
	zip -j $(NAME).zip $^ ../doc/dokumentace.pdf
