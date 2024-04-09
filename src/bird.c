#include "sprites.h"
#include "bird.h"
#include "game.h"

/* initialize the bird */
void bird_init(Bird* bird) {
    // TODO: add and remove fields that are neccessary or unnecessary
    bird->x = 100;
    bird->y = 113;
    bird->border = 40;
    bird->frame = 0;
    bird->move = 0;
    bird->counter = 0;
    bird->animation_delay = 8;
    bird->sprite = sprite_init(bird->x, bird->y, SIZE_16_32, 0, 0, bird->frame, 0);
}

// TODO: change to go up
int bird_right(Bird* bird) {
    /* face right */
    sprite_set_horizontal_flip(bird->sprite, 0);
    bird->move = 1;

    /* if we are at the right end, just scroll the screen */
    if (bird->x > (SCREEN_WIDTH - 16 - bird->border)) {
        return 1;
    } else {
        /* else move right */
        bird->x++;
        return 0;
    }
}

// maybe unnecessary
void bird_stop(Bird* bird) {
    bird->move = 0;
    bird->frame = 0;
    bird->counter = 7;
    sprite_set_offset(bird->sprite, bird->frame);
}

/* update the bird */
void bird_update(Bird* bird) {
    if (bird->move) {
        bird->counter++;
        if (bird->counter >= bird->animation_delay) {
            bird->frame = bird->frame + 16;
            if (bird->frame > 16) {
                bird->frame = 0;
            }
            sprite_set_offset(bird->sprite, bird->frame);
            bird->counter = 0;
        }
    }

    sprite_position(bird->sprite, bird->x, bird->y);
}
