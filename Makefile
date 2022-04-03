SRC=$(wildcard *.c)
OBJS=$(SRC:.c=.o)
LIBS=-lncursesw
CFLAGS=-Wall -Werror -Wpedantic -g
EXEC=solitaire
CC=gcc
DEPS=$(wildcard *.h)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(LIBS) $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJS)
