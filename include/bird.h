#ifndef BIRD_H
#define BIRD_H

typedef struct {
  int flap_delta;
  int gravity;
  int is_falling;
  int x;
  int y;
  int is_dead;
  int touched_portal;
} Bird;

#endif
