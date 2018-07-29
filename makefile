CC=		gcc
CFLAG=	-Wall -Wextra -g
LDFLAGS=	-lncurses
minesweeper:	minesweeper.c
		$(CC) -o minesweeper minesweeper.c $(CFLAGS) $(LDFLAGS)
