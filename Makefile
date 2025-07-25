# Makefile for snake.c (Windows)
# 使用 gcc 编译 snake.c 为 snake.exe

CC = gcc
SRC = snake_game.c
OUT = snake.exe
CFLAGS = -Wall -O2

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

run: $(OUT)
	./$(OUT)

clean:
	del $(OUT)
