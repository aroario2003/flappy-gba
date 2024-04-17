#ifndef PIPE_H
#define PIPE_H

struct Pipe {
    Sprite* sprite;
    int x;
    int y;
    int pipeType;
    int speed;
};

void pipe_init(struct Pipe* pipe, int x, int y, int pipeType, int speed);
void pipe_move(struct Pipe pipe);
void pipe_collisions(struct Pipe* pipe);

#endif
