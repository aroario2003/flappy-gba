#include "lib.h"
#include "sprites.h"
#include "player.h"
volatile unsigned short* sprite_palette = (volatile unsigned short*) 0x5000200;
int next_sprite_index = 0;
#define PALETTE_SIZE 256
volatile unsigned short* sprite_attribute_memory = (volatile unsigned short*) 0x7000000;
volatile unsigned short* sprite_image_memory = (volatile unsigned short*) 0x6010000;
struct Sprite;
void sprite_update_all() {
    /* copy them all over */
    memcpy16_dma((unsigned short*) sprite_attribute_memory, (unsigned short*) sprites, NUM_SPRITES * 4);
}
/* change the tile offset of a sprite */
void sprite_set_offset(Sprite* sprite, int offset) {
    /* clear the old offset */
    sprite->attribute2 &= 0xfc00;

    /* apply the new one */
    sprite->attribute2 |= (offset & 0x03ff);
}

/* setup the sprite image and palette */
void setup_sprite_image() {
    /* load the palette from the image into palette memory*/
    memcpy16_dma((unsigned short*) sprite_palette, (unsigned short*) player_palette, PALETTE_SIZE);

    /* load the image into sprite image memory */
    memcpy16_dma((unsigned short*) sprite_image_memory, (unsigned short*) player_data, (player_width * player_height) / 2);
}
/* function to initialize a sprite with its properties, and return a pointer */
Sprite* sprite_init(int x, int y, SpriteSize size,
        int horizontal_flip, int vertical_flip, int tile_index, int priority) {

    /* grab the next index */
    int index = next_sprite_index++;
    int size_bits, shape_bits;
    switch (size) {
        case SIZE_8_8:   size_bits = 0; shape_bits = 0; break;
        case SIZE_16_16: size_bits = 1; shape_bits = 0; break;
        case SIZE_32_32: size_bits = 2; shape_bits = 0; break;
        case SIZE_64_64: size_bits = 3; shape_bits = 0; break;
        case SIZE_16_8:  size_bits = 0; shape_bits = 1; break;
        case SIZE_32_8:  size_bits = 1; shape_bits = 1; break;
        case SIZE_32_16: size_bits = 2; shape_bits = 1; break;
        case SIZE_64_32: size_bits = 3; shape_bits = 1; break;
        case SIZE_8_16:  size_bits = 0; shape_bits = 2; break;
        case SIZE_8_32:  size_bits = 1; shape_bits = 2; break;
        case SIZE_16_32: size_bits = 2; shape_bits = 2; break;
        case SIZE_32_64: size_bits = 3; shape_bits = 2; break;
    }

    int h = horizontal_flip ? 1 : 0;
    int v = vertical_flip ? 1 : 0;
//Set to 16x16
//Sprite offset: 4 tiles per sprite, 2x2, (0, 8, 16, 24)
    /* set up the first attribute */
    sprites[index].attribute0 = y     |         /* y coordinate */
                            (0 << 8)  |         /* rendering mode */
                            (0 << 10) |         /* gfx mode */
                            (0 << 12) |         /* mosaic */
                            (1 << 13) |         /* color mode, 0:16, 1:256 */
                            (shape_bits << 14); /* shape*/

    /* set up the second attribute */
    sprites[index].attribute1 = x     |         /* x coordinate */
                            (0 << 9)  |         /* affine flag */
                            (h << 12) |         /* horizontal flip flag */
                            (v << 13) |         /* vertical flip flag */
                            (size_bits << 14);  /* size */

    /* setup the second attribute */
    sprites[index].attribute2 = tile_index   |  /* tile index */
                            (priority << 10) |  /* priority */
                            (0 << 12);          /* palette bank (only 16 color)*/

    /* return pointer to this sprite */
    return &sprites[index];
}

/* set a sprite position */
void sprite_position(Sprite* sprite, int x, int y) {
    /* clear out the y coordinate */
    sprite->attribute0 &= 0xff00;

    /* set the new y coordinate */
    sprite->attribute0 |= (y & 0xff);

    /* clear out the x coordinate */
    sprite->attribute1 &= 0xfe00;

    /* set the new x coordinate */
    sprite->attribute1 |= (x & 0x1ff);
}

/* move a sprite in a direction */
void sprite_move(Sprite* sprite, int dx, int dy) {
    /* get the current y coordinate */
    int y = sprite->attribute0 & 0xff;

    /* get the current x coordinate */
    int x = sprite->attribute1 & 0x1ff;

    /* move to the new location */
    sprite_position(sprite, x + dx, y + dy);
}

/* change the vertical flip flag */
void sprite_set_vertical_flip(Sprite* sprite, int vertical_flip) {
    if (vertical_flip) {
        /* set the bit */
        sprite->attribute1 |= 0x2000;
    } else {
        /* clear the bit */
        sprite->attribute1 &= 0xdfff;
    }
}

/* change the vertical flip flag */
void sprite_set_horizontal_flip(Sprite* sprite, int horizontal_flip) {
    if (horizontal_flip) {
        /* set the bit */
        sprite->attribute1 |= 0x1000;
    } else {
        /* clear the bit */
        sprite->attribute1 &= 0xefff;
    }
}
