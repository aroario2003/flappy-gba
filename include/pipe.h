#ifndef PIPE_H
#define PIPE_H

typedef struct {
    Sprite* sprite;
    int x;
    int y;
    int pipeType;
    int speed;
} Pipe;

void pipe_init(Pipe* pipe, int x, int y, int pipeType, int speed);
void pipe_move(Pipe* pipe);
void pipe_collisions(Pipe* pipe);

#endif
