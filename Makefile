OBJ = main.o fileio.o gapbuffer.o
CFLAGS = -g -Wall -fsanitize=address
CC = clang

all: jedit

jedit: ${OBJ}
	${CC} ${CFLAGS} -o $@ $^ -lcurses

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	rm ${OBJ} jedit
