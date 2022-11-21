CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror

SRCS=$(wildcard *.c)
HEADERS=$(wildcard *.h)
OBJS=$(SRCS:%.c=%.o)

all: ifj22

ifj22: $(OBJS)
	$(CC) $(CFLAGS) -g -o $@ $^ $(LFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -g -c $< -o $@

clean:
	rm -rf *.o
	rm ifj22
	rm xbarto0g.zip

pack: $(SRCS) $(HEADERS) Makefile rozdeleni rozsireni
	zip xbarto0g.zip $^
