CC = gcc
CFLAGS = -Wall -std=c99 -g

all: gomoku renju replay

gomoku: gomoku.o game.o io.o board.o
	gcc gomoku.o game.o io.o board.o -o gomoku

renju: renju.o game.o io.o board.o
	gcc renju.o game.o io.o board.o -o renju

replay: replay.o game.o io.o board.o
	gcc replay.o game.o io.o board.o -o replay

# Each Object File
gomoku.o: gomoku.c game.h io.h
renju.o: renju.c game.h io.h
replay.o: replay.c game.h io.h
game.o: game.c game.h board.h
io.o: io.c game.h board.h
board.o: board.c board.h

clean: 
	rm -f game.o io.o board.o gomoku.o replay.o renju.o
	rm -f gomoku renju replay
	rm -f output.txt