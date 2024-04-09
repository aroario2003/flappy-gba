#include "sprites.h"

#ifndef BIRD_H
#define BIRD_H

typedef struct {
  Sprite* sprite;
  int flap_delta;
  int gravity;
  int is_falling;
  int x;
  int y;
  int is_dead;
  int touched_portal;
} Bird;

#endif
