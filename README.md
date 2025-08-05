# Project 1 - Breakout Game with raylib

A simple breakout-style game written in C using the [raylib](https://www.raylib.com/) library.

# Features

- Classic Breakout gameplay with paddle, ball, and bricks
- Three levels with different brick colors and ball speeds
- Sound effects for paddle hit, brick break, game over, and menu clicks
- Sound toggle ON/OFF in the menu
- Navigate menu using keyboard (UP/DOWN/LEFT/RIGHT/ENTER) or mouse
- ESC key returns to menu instead of closing the window
- Score and lives display
- Two types of bricks per level with distinct colors

# Requirements

- [raylib](https://www.raylib.com/) installed and properly configured
- C compiler (e.g., GCC)
- Audio device enabled (for sound playback)

# Sound Files Required

Place these files in the same directory as the executable:

- `paddle.mp3`
- `brick.wav`
- `gameover.wav`
- `click.wav`

# How to Build

Example compilation command (on Linux/macOS with raylib installed):

```bash
gcc -o breakout breakout.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lasound
