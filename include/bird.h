#ifndef BIRD_H
#define BIRD_H

typedef struct{
  Sprite* sprite;
  int flap_delta;
  int gravity;
  int is_falling;
  int x;
  int y;
  int is_dead;
  int border;
  int frame;
  int move;
  int counter;
  int animation_delay;
  int touched_portal;
} Bird;

void bird_init(Bird* bird);
int bird_right(Bird* bird);
void bird_stop(Bird* bird);
void bird_update(Bird* bird);
void bird_up(Bird* bird);

#endif
