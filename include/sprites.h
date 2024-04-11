#ifndef SPRITES_H
#define SPRITES_H

#define NUM_SPRITES 128

typedef struct {
    unsigned short attribute0;
    unsigned short attribute1;
    unsigned short attribute2;
    unsigned short attribute3;
} Sprite;

/* array of all the sprites available on the GBA */
Sprite sprites[NUM_SPRITES];
int next_sprite_index = 0;

typedef enum {
    SIZE_8_8,
    SIZE_16_16,
    SIZE_32_32,
    SIZE_64_64,
    SIZE_16_8,
    SIZE_32_8,
    SIZE_32_16,
    SIZE_64_32,
    SIZE_8_16,
    SIZE_8_32,
    SIZE_16_32,
    SIZE_32_64
} SpriteSize;

volatile unsigned short* sprite_attribute_memory = (volatile unsigned short*) 0x7000000;
volatile unsigned short* sprite_image_memory = (volatile unsigned short*) 0x6010000;

Sprite* sprite_init(int x, int y, SpriteSize size, int horizontal_flip, int vertical_flip, int tile_index, int priority);
void sprite_position(Sprite* sprite, int x, int y);
void sprite_move(Sprite* sprite, int dx, int dy);
void sprite_set_vertical_flip(Sprite* sprite, int vertical_flip);
void sprite_set_horizontal_flip(Sprite* sprite, int horizontal_flip);
void sprite_set_offset(Sprite* sprite, int offset);
void sprite_update_all();

#endif
