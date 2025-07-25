# Snake Game (C Version)

A simple snake game written in C, using Windows console APIs.

## Features

- Snake movement using WASD keys  
- Fruit spawning and growth  
- Wrapping around edges  
- Game speed control  

## How to Build

Make sure you have a C compiler installed (e.g., GCC with MinGW on Windows).

### Compile using Makefile

```bash
make
```

Or compile manually with GCC:
```bash
gcc -Wall -O2 snake_game.c -o snake_game.exe
```

## Controls
- W, A, S, D — Move the snake
- ESC — Quit the game
