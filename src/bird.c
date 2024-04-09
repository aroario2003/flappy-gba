    //Included 'sprite.c' as it could not access sprite commands called -M
#include "../src/sprite.c"
#include "bird.h"
struct Bird;
/* initialize the bird */

    //Changed 'Bird*' to 'Bird" due to deref issues with every single variable within -M
void bird_init(struct Bird bird) {
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
    //Changed 'Bird*' to 'Bird" due to deref issues with every single variable     within -M

int bird_right(struct Bird bird) {
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
    //Changed 'Bird*' to 'Bird" due to deref issues with every single variable     within -M

void bird_stop(struct Bird bird) {
    bird->move = 0;
    bird->frame = 0;
    bird->counter = 7;
    sprite_set_offset(bird->sprite, bird->frame);
}

/* update the bird */
    //Changed 'Bird*' to 'Bird" due to deref issues with every single variable     within -M

void bird_update(struct Bird bird) {
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
