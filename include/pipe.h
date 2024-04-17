#ifndef PIPE_H
#define PIPE_H

 typedef struct {
     Sprite* sprite;
     int x;
     //Each pipe will be made of stitched sprites. y is for which layer it ison.
     int y;
     //Used to check which sprite to use. 8 for upwards opening, 9 for downwards opening, 10 for no opening. Only used for when the sprite is being initialized
     int pipeType;
     //Used for how fast it moves. To be added to the 'x' position each frame it moves during.
     int speed;
 } Pipe;

//Create the pipe sprite.
void pipe_init(int x, int y, int pipeType, int speed);
// Move it.
void pipe_move(int x, int speed);
//Check if it collides with the bird each frame.
void pipe_collisions(int x, int y);

#endif
