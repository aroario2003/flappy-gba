#ifndef GAME_H
#define GAME_H

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160

#define PALETTE_SIZE 256

typedef struct {
  int game_started;
  int game_in_progress;
  int game_ended;
} Game;

#endif
