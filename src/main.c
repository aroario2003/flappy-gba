
//#include "taptapextended.h"
//#include "taptap.h"
//#include "player.h"
#include "flappin.h"
#include "background.h"
#include "background_night.h"
#include "background_dusk.h"
#include "map.h"
#include "map2.h"
#include "hud.h"
// SCREEN_WIDTH and SCREEN_HEIGHT defined here (below)
#include "game.h"
#include "font.h"
//#include "fb_song.h"
#include "sprites.h"
#include "bird.h"
#include "pipe.h"
#include "scores.h"
//#include "starting_screen.h"
#include "end_screen_tiles.h"
#include "end_game.h"
#include "stdio.h"
#include "font_and_background.h"
#include "numbers.h"
// #include "starting_screen2.h"

/* the three tile modes */
#define MODE0 0x00
#define MODE1 0x01
#define MODE2 0x02
#define MODE3 0X0003
#define BG2 0x0400

/* enable bits for the four tile layers */
#define BG0_ENABLE 0x100
#define BG1_ENABLE 0x200
#define BG2_ENABLE 0x400
#define BG3_ENABLE 0x800

/* needed for start screen */
#define WIDTH 240
#define HEIGHT 160

#define SPRITE_ENABLE 0x1000
#define SPRITE_MAP_1D 0x40


//These are the codes for the tiles. To be used to edit h files.
//TODO: Set it so the tiles change with time. As of right now, it should be the first three positions in the array.
// test comment
#define TILE_0 0x0023
#define TILE_1 0x0024
#define TILE_2 0x0025
#define TILE_3 0x0026
#define TILE_4 0x0027
#define TILE_5 0x0028
#define TILE_6 0x0029
#define TILE_7 0x002a
#define TILE_8 0x002b
#define TILE_9 0x002c
/* the control registers for the four tile layers */
volatile unsigned short* screen = (volatile unsigned short*) 0x6000000;
volatile unsigned short* bg0_control = (volatile unsigned short*) 0x4000008;
volatile unsigned short* bg1_control = (volatile unsigned short*) 0x400000a;
volatile unsigned short* bg2_control = (volatile unsigned short*) 0x400000c;
volatile unsigned short* bg3_control = (volatile unsigned short*) 0x400000e;

/* the display control pointer points to the gba graphics register */
volatile unsigned long* display_control = (volatile unsigned long*) 0x4000000;

/* the address of the color palette */
volatile unsigned short* bg_palette = (volatile unsigned short*) 0x5000000;

/* the button register holds the bits which indicate whether each button has
 * been pressed - this has got to be volatile as well
 */
volatile unsigned short* buttons = (volatile unsigned short*) 0x04000130;

/* scrolling registers for backgrounds */
volatile short* bg0_x_scroll = (unsigned short*) 0x4000010;
volatile short* bg0_y_scroll = (unsigned short*) 0x4000012;
volatile short* bg1_x_scroll = (unsigned short*) 0x4000014;
volatile short* bg1_y_scroll = (unsigned short*) 0x4000016;
volatile short* bg2_x_scroll = (unsigned short*) 0x4000018;
volatile short* bg2_y_scroll = (unsigned short*) 0x400001a;
volatile short* bg3_x_scroll = (unsigned short*) 0x400001c;
volatile short* bg3_y_scroll = (unsigned short*) 0x400001e;


/* the bit positions indicate each button - the first bit is for A, second for
 * B, and so on, each constant below can be ANDED into the register to get the
 * status of any one button */
#define BUTTON_A (1 << 0)
#define BUTTON_B (1 << 1)
#define BUTTON_SELECT (1 << 2)
#define BUTTON_START (1 << 3)
#define BUTTON_RIGHT (1 << 4)
#define BUTTON_LEFT (1 << 5)
#define BUTTON_UP (1 << 6)
#define BUTTON_DOWN (1 << 7)
#define BUTTON_R (1 << 8)
#define BUTTON_L (1 << 9)

/* the scanline counter is a memory cell which is updated to indicate how
 * much of the screen has been drawn */
volatile unsigned short* scanline_counter = (volatile unsigned short*) 0x4000006;

/* define the timer control registers */
volatile unsigned short* timer0_data = (volatile unsigned short*) 0x4000100;
volatile unsigned short* timer0_control = (volatile unsigned short*) 0x4000102;

/* make defines for the bit positions of the control register */
#define TIMER_FREQ_1 0x0
#define TIMER_FREQ_64 0x2
#define TIMER_FREQ_256 0x3
#define TIMER_FREQ_1024 0x4
#define TIMER_ENABLE 0x80

/* the GBA clock speed is fixed at this rate */
#define CLOCK 16777216
#define CYCLES_PER_BLANK 280806

/* turn DMA on for different sizes */
#define DMA_ENABLE 0x80000000
#define DMA_16 0x00000000
#define DMA_32 0x04000000

/* this causes the DMA destination to be the same each time rather than increment */
#define DMA_DEST_FIXED 0x400000

/* this causes the DMA to repeat the transfer automatically on some interval */
#define DMA_REPEAT 0x2000000

/* this causes the DMA repeat interval to be synced with timer 0 */
#define DMA_SYNC_TO_TIMER 0x30000000

/* pointers to the DMA source/dest locations and control registers */
volatile unsigned int* dma1_source = (volatile unsigned int*) 0x40000BC;
volatile unsigned int* dma1_destination = (volatile unsigned int*) 0x40000C0;
volatile unsigned int* dma1_control = (volatile unsigned int*) 0x40000C4;

volatile unsigned int* dma2_source = (volatile unsigned int*) 0x40000C8;
volatile unsigned int* dma2_destination = (volatile unsigned int*) 0x40000CC;
volatile unsigned int* dma2_control = (volatile unsigned int*) 0x40000D0;

volatile unsigned int* dma3_source = (volatile unsigned int*) 0x40000D4;
volatile unsigned int* dma3_destination = (volatile unsigned int*) 0x40000D8;
volatile unsigned int* dma3_control = (volatile unsigned int*) 0x40000DC;


/* the global interrupt enable register */
volatile unsigned short* interrupt_enable = (unsigned short*) 0x4000208;

/* this register stores the individual interrupts we want */
volatile unsigned short* interrupt_selection = (unsigned short*) 0x4000200;

/* this registers stores which interrupts if any occured */
volatile unsigned short* interrupt_state = (unsigned short*) 0x4000202;

/* the address of the function to call when an interrupt occurs */
volatile unsigned int* interrupt_callback = (unsigned int*) 0x3007FFC;

/* this register needs a bit set to tell the hardware to send the vblank interrupt */
volatile unsigned short* display_interrupts = (unsigned short*) 0x4000004;

/* the interrupts are identified by number, we only care about this one */
#define INTERRUPT_VBLANK 0x1

/* allows turning on and off sound for the GBA altogether */
volatile unsigned short* master_sound = (volatile unsigned short*) 0x4000084;
#define SOUND_MASTER_ENABLE 0x80
#define SOUND_MASTER_DISABLE (~SOUND_MASTER_ENABLE)

/* has various bits for controlling the direct sound channels */
volatile unsigned short* sound_control = (volatile unsigned short*) 0x4000082;

/* bit patterns for the sound control register */
#define SOUND_A_RIGHT_CHANNEL 0x100
#define SOUND_A_LEFT_CHANNEL 0x200
#define SOUND_A_FIFO_RESET 0x800
#define SOUND_B_RIGHT_CHANNEL 0x1000
#define SOUND_B_LEFT_CHANNEL 0x2000
#define SOUND_B_FIFO_RESET 0x8000

/* the location of where sound samples are placed for each channel */
volatile unsigned char* fifo_buffer_a  = (volatile unsigned char*) 0x40000A0;
volatile unsigned char* fifo_buffer_b  = (volatile unsigned char*) 0x40000A4;

/* global variables to keep track of how much longer the sounds are to play */
unsigned int channel_a_vblanks_remaining = 0;
unsigned int channel_a_total_vblanks = 0;
unsigned int channel_b_vblanks_remaining = 0;



/* wait for the screen to be fully drawn so we can do something during vblank */
void wait_vblank() {
  /* wait until all 160 lines have been updated */
  while (*scanline_counter < 160) { }
}


/* this function checks whether a particular button has been pressed */
unsigned char button_pressed(unsigned short button) {
  /* and the button register with the button constant we want */
  unsigned short pressed = *buttons & button;

  /* if this value is zero, then it's not pressed */
  if (pressed == 0) {
    return 1;
  } else {
    return 0;
  }
}


/* return a pointer to one of the 4 character blocks (0-3) */
volatile unsigned short* char_block(unsigned long block) {
  /* they are each 16K big */
  return (volatile unsigned short*) (0x6000000 + (block * 0x4000));
}

/* return a pointer to one of the 32 screen blocks (0-31) */
volatile unsigned short* screen_block(unsigned long block) {
  /* they are each 2K big */
  return (volatile unsigned short*) (0x6000000 + (block * 0x800));
}


/* function to setup background 0 for this program */
//The 'mode' should be 0 for day, 1 for sunset, and 1 for night. Does not auto-set right now.
void setup_background(int mode) {

  /* load the palette from the image into palette memory*/
  for (int i = 0; i < PALETTE_SIZE; i++) {
    if (mode==0){
      bg_palette[i] = font_and_background_palette[i];
    }
    else if (mode==1){
      bg_palette[i] = background_dusk_palette[i];
    }
    else if (mode==2){
      bg_palette[i] = background_night_palette[i];
    }
  }

  /* load the image into char block 0 (16 bits at a time) */
  volatile unsigned short* dest = char_block(0);
  unsigned short* image = (unsigned short*) background_data;
  for (int i = 0; i < ((background_width * background_height) / 2); i++) {
    dest[i] = image[i];
  }

  /* set all control the bits in this register */
  *bg0_control = 2 |    /* priority, 0 is highest, 3 is lowest */
    (0 << 2)  |       /* the char block the image data is stored in */
    (0 << 6)  |       /* the mosaic flag */
    (1 << 7)  |       /* color mode, 0 is 16 colors, 1 is 256 colors */
    (16 << 8) |       /* the screen block the tile data is stored in */
    (1 << 13) |       /* wrapping flag */
    (0 << 14);        /* bg size, 0 is 256x256 */

  *bg1_control = 1 |
    (0 << 2)  |
    (0 << 6)  |
    (1 << 7)  |
    (17 << 8) |
    (1 << 13) |
    (0 << 14);

  /* load the tile data into screen block 16 */
  dest = screen_block(16);
  for (int i = 0; i < (map_width * map_height); i++) {
    dest[i] = map[i];
  }
  /* load the tile data into screen block 17 */
  dest = screen_block(17);
  for (int i = 0; i < (map_width * map_height); i++) {
    dest[i] = map2[i];
  }
   *bg2_control = 0 |
    (1 << 2)  |
   (0 << 6)  |
  (1 << 7)  |
  (18 << 8) |
    (1 << 13) |
  (0 << 14);

  dest = char_block(1);
  image = (unsigned short*) background_dusk_data;
  for (int i = 0; i < ((background_dusk_width * background_dusk_height) / 2); i++) {
    dest[i] = hud[i];
  }
/*
  *bg2_control = 0 |
    (1 << 2)  |
    (0 << 6)  |
    (1 << 7)  |
    (18 << 8) |
    (1 << 13) |
    (0 << 14);
*/
//  dest = screen_block(24);
//  for (int i = 0; i < 32 * 32; i++) {
//      dest[i] = 1;
//  }
}

void set_text(char* str, int row, int col) {
  /* find the index in the texmap to draw to */
  int index = row * 32 + col;
  
  int missing = 32;

  /* pointer to text map */
  volatile unsigned short* ptr = screen_block(24);

  /* for each character */
  while (*str) {
    /* place this character in the map */
    ptr[index] = *str - missing;

    /* move onto the next character */
    index++;
    str++;
  }   
}

/* just kill time */
void delay(unsigned int amount) {
  for (int i = 0; i < amount * 10; i++);
}

void put_pixel_m3(int row, int col, unsigned short color) {
  /* set the screen location to this color */
  screen[row * WIDTH + col] = color;
}

void handle_start() {
  while(!button_pressed(BUTTON_START)) {}

}
//Creates a pipe.
void pipe_init(Pipe *pipe, int x, int y, int pipeType, int speed){
  pipe->x = x;
  pipe->y = y;
  pipe->speed = speed;
  pipe->pipeType = pipeType;
  //pipe->sprite=sprite_init(pipe->x, pipe->y, SIZE_16_16, 0, 0, pipe->pipeType, 0);
}

//Moves a pipe each time it is called.
void pipe_move(Pipe *pipe){
  pipe->x = pipe->x + pipe->speed;
  sprite_move(pipe->sprite, pipe->speed, 0);

}

int pipe_collisions(Pipe* pipe, Bird* bird){
  int birdLocation = bird->y; 
  int pipeLocationTop = pipe->y;
  int pipeLocationBottom = pipe->y-8;
}

void setup_endscreen(char* score_str, char* coin_str) {
  volatile unsigned short* dest = char_block(0);

  /* works somewhat */
  unsigned short* image = (unsigned short*) end_screen_tiles_data;
  for (int i = 0; i < ((end_screen_tiles_width * end_screen_tiles_height) / 2); i++) {
    dest[i] = image[i];
  }

  /* load palette supporting background and ending screen */
  for (int i = 0; i < PALETTE_SIZE; i++) {
    bg_palette[i] = end_screen_tiles_palette[i];
  }

  *bg2_control = 0 |
    (0 << 2)  |
    (0 << 6)  |
    (1 << 7)  |
    (18 << 8) |
    (1 << 13) |
    (0 << 14);

  /* load the tile data into screen block 18 */
  dest = screen_block(18);
  for (int i = 0; i < (end_game_width * end_game_height); i++) {
    dest[i] = end_game[i];
  }

}

// return 1 if conflict, 0 if no conflict
int get_borders_and_determine_conflict(int x, int y, int pipeNumber, int pipeX) {
  if (pipeNumber == 1) {
    // safezone: pixel Y 87 to pixel Y 118 (where edge of bird is not touching the pipe)
    // no conflict cases
    if (x+11 < pipeX || x > pipeX+13) {
      return 0;
    }
    if (y >= 87 && y <= 118-11) {
      return 0;
    }
    return 1;
  }
  else if (pipeNumber == 2) {
    // safezone: pixel Y 47 to Pixel Y 78
    // no conflict cases
    if (x+11 < pipeX || x > pipeX+13) {
      return 0;
    }
    if (y >= 47 && y <= 78-11) {
      return 0;
    }
    return 1;
  }
}

// return 1 if conflict, 0 if no conflict
int get_coin_borders_and_determine_conflict(int x, int y, int coinNumber, int coinX) {
    // adjust coinX by +2 as game rendering is different than real math
    coinX +=2;
    if (coinNumber == 1) {
        // deathzone: pixel Y 78 to pixel Y 102 (where edge of bird touches coin)
        // conflict case
        if (y >= 78 && y <= 102 && x >= coinX-14 && x <= coinX+12) {
            return 1;
        }
        return 0;
    }
    else if (coinNumber == 2) {
        // deathzone: pixel Y 38 to pixel Y 62 (where edge of bird touches coin)
        // conflict case
        if (y >= 38 && y <= 62 && x >= coinX-14 && x <= coinX+12) {
            return 1;
        }
        return 0;
    }
}

/*
void play_sound(const signed char* sound, int total_samples, int sample_rate, char channel) {
    *timer0_control = 0;
    if (channel == 'A') {
        *dma1_control = 0;
    } else if (channel == 'B') {
        *dma2_control = 0;
    }

    if (channel == 'A') {
        *sound_control |= SOUND_A_RIGHT_CHANNEL | SOUND_A_LEFT_CHANNEL | SOUND_A_FIFO_RESET;
    } else if (channel == 'B') {
        *sound_control |= SOUND_B_RIGHT_CHANNEL | SOUND_B_LEFT_CHANNEL | SOUND_B_FIFO_RESET;
    }

    *master_sound = SOUND_MASTER_ENABLE;

    if (channel == 'A') {
        *dma1_source = (unsigned int) sound;
        *dma1_destination = (unsigned int) fifo_buffer_a;
        *dma1_control = DMA_DEST_FIXED | DMA_REPEAT | DMA_32 | DMA_SYNC_TO_TIMER | DMA_ENABLE;
    } else if (channel == 'B') {
        *dma2_source = (unsigned int) sound;
        *dma2_destination = (unsigned int) fifo_buffer_b;
        *dma2_control = DMA_DEST_FIXED | DMA_REPEAT | DMA_32 | DMA_SYNC_TO_TIMER | DMA_ENABLE;
    }

    unsigned short ticks_per_sample = CLOCK / sample_rate;

    *timer0_data = 65536 - ticks_per_sample;

    if (channel == 'A') {
        channel_a_vblanks_remaining = total_samples * ticks_per_sample * (1.0 / CYCLES_PER_BLANK);
        channel_a_total_vblanks = channel_a_vblanks_remaining;
    } else if (channel == 'B') {
        channel_b_vblanks_remaining = total_samples * ticks_per_sample * (1.0 / CYCLES_PER_BLANK);
    }

    *timer0_control = TIMER_ENABLE | TIMER_FREQ_1;
}

void on_vblank() {
    *interrupt_enable = 0;
    unsigned short temp = *interrupt_state;

    if ((*interrupt_state & INTERRUPT_VBLANK) == INTERRUPT_VBLANK) {

        if (channel_a_vblanks_remaining == 0) {
            channel_a_vblanks_remaining = channel_a_total_vblanks;
            *dma1_control = 0;
            *dma1_source = (unsigned int) taptap;
            *dma1_control = DMA_DEST_FIXED | DMA_REPEAT | DMA_32 |
                DMA_SYNC_TO_TIMER | DMA_ENABLE;
        } else {
            channel_a_vblanks_remaining--;
        }

        if (channel_b_vblanks_remaining == 0) {
            *sound_control &= ~(SOUND_B_RIGHT_CHANNEL | SOUND_B_LEFT_CHANNEL | SOUND_B_FIFO_RESET);
            *dma2_control = 0;
        }
        else {
            channel_b_vblanks_remaining--;
        }
    }

    *interrupt_state = temp;
    *interrupt_enable = 1;
}
*/


/* the main function */
int main() {
  *display_control = MODE3 | BG2;
  for (int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      put_pixel_m3(row, col, flappin_data[row * WIDTH + col]);
    }
  }
  Sprite* theSprite;
  theSprite = sprite_init(100, 100, SIZE_16_16, 0, 0, 12, 0);

  handle_start();
  /* we set the mode to mode 0 with bg0 on */
  *display_control = MODE0 | BG0_ENABLE | BG1_ENABLE | SPRITE_ENABLE | SPRITE_MAP_1D;
  /* store ints if game has started or is in progress or has ended. 0 = false, 1 = true */
  int game_started = 0;
  int game_in_progress = 1;
  int game_ended = 0;
	
  Scores* scores;
  scores->points = 0;
  scores->coins = 0;
  
  *sound_control=0;
  /* setup the background 0 */
  setup_background(1);
  //Will save the value of the last background. 
  //Used to change if the background changes in-game.
  int lastBackground = 1;
  int currentBackground =1;
  /* set initial scroll to 0 */
  int xscroll = 0;
  int yscroll = 0;
  setup_sprite_image();
  sprite_clear();
  Bird* bird;
  bird_init(bird);
  //Counter. Counts the amount of ticks we have gone along.
  int counter = 0;
  //PIPE 1
  Sprite* sprite2;
  Sprite* sprite3;
  Sprite* sprite4;
  Sprite* sprite5;
  Sprite* sprite6;
  Sprite* sprite7;
  Sprite* sprite8;
  Sprite* sprite9;
  Sprite* spritea;
  Sprite* spriteb;
  Sprite* spritec;
  Sprite* sprited;
  spritea=sprite_init(WIDTH, HEIGHT-10, SIZE_16_16, 0, 0, 64, 0);
  spriteb=sprite_init(WIDTH, HEIGHT-20, SIZE_16_16, 0, 0, 64, 0);
  spritec=sprite_init(WIDTH, HEIGHT-30, SIZE_16_16, 0, 0, 64, 0);
  sprited=sprite_init(WIDTH, HEIGHT-40, SIZE_16_16, 0, 0, 64, 0);
  sprite4 = sprite_init(WIDTH, 0, SIZE_16_16, 0, 0, 64, 0);
  sprite3 = sprite_init(WIDTH, 10, SIZE_16_16, 0, 0, 64, 0);
  sprite2 = sprite_init(WIDTH, 20, SIZE_16_16, 0, 0, 64, 0);
  sprite5 = sprite_init(WIDTH, 30, SIZE_16_16, 0, 0, 64, 0);
  sprite6 = sprite_init(WIDTH, 40, SIZE_16_16, 0, 0, 64, 0);
  sprite7 = sprite_init(WIDTH, 50, SIZE_16_16, 0, 0, 64, 0);
  sprite8 = sprite_init(WIDTH, 60, SIZE_16_16, 0, 0, 64, 0);
  sprite9 = sprite_init(WIDTH, 70, SIZE_16_16, 0, 0, 64, 0);

  //PIPE 2
  Sprite* s1prite;
  Sprite* s2prite;
  Sprite* s3prite;
  Sprite* s4prite;
  Sprite* s5prite;
  Sprite* s6prite;
  Sprite* s7prite;
  Sprite* s8prite;
  Sprite* s9prite;
  Sprite* asprite;
  Sprite* bsprite;
  Sprite* csprite;
  Sprite* dsprite;

  s1prite = sprite_init(0, HEIGHT-10, SIZE_16_16, 0, 0, 64, 0);
  s2prite = sprite_init(0, HEIGHT-20, SIZE_16_16, 0, 0, 64, 0);
  s3prite = sprite_init(0, HEIGHT-30, SIZE_16_16, 0, 0, 64, 0);
  s4prite = sprite_init(0, HEIGHT-40, SIZE_16_16, 0, 0, 64, 0);
  s6prite = sprite_init(0, HEIGHT-50, SIZE_16_16, 0, 0, 64, 0);
  s7prite = sprite_init(0, HEIGHT-60, SIZE_16_16, 0, 0, 64, 0);
  s8prite = sprite_init(0, HEIGHT-70, SIZE_16_16, 0, 0, 64, 0);
  s9prite = sprite_init(0, HEIGHT-80, SIZE_16_16, 0, 0, 64, 0);
  asprite = sprite_init(0, 10, SIZE_16_16, 0, 0, 64, 0);
  bsprite = sprite_init(0, 20, SIZE_16_16, 0, 0, 64, 0);
  csprite = sprite_init(0, 30, SIZE_16_16, 0, 0, 64, 0);
  dsprite = sprite_init(0, 0, SIZE_16_16, 0, 0, 64, 0);

  Sprite* coin2;
     coin2 = sprite_init(0, 50, SIZE_16_16, 0, 0, 80, 0);
     int coin2Positiony;
     int coin2Positionx=HEIGHT;
  Sprite* coin;
     coin = sprite_init(WIDTH, 90, SIZE_16_16, 0, 0, 80, 0);
     int coinPositiony=WIDTH;
     int coinPositionx=HEIGHT;
  int spriteMode = 0;
  //play_sound(taptap, taptap_bytes, 16000, 'A');

  /* loop forever */
  while (1) {
    //Kill switch 
    if (counter%60 == 0){
      if (spriteMode == 0){
	sprite_set_offset(theSprite, 8);
	spriteMode = 8;
      }
      else{
	sprite_set_offset(theSprite, 0);
	spriteMode = 0;
      }
    }
    if (button_pressed(BUTTON_SELECT) && game_ended == 0) {
      *display_control = MODE0 | BG0_ENABLE | BG1_ENABLE | BG2_ENABLE;
      char score_str[100];
      char coin_str[100];
      sprintf(score_str, "you got %d points", scores->points);
      sprintf(coin_str, "you got %d coins", scores->coins);
      setup_endscreen(score_str, coin_str);
      game_ended = 1;
      game_in_progress = 0;
    }
    //Start again if you die
    if (button_pressed(BUTTON_START) && game_ended == 1) {
      game_started = 1;
      game_ended = 0;
      counter = 0;
      break;
    }
           
    //TODO: Make bird move properly
    if (button_pressed(BUTTON_A)&& counter%2 == 0){
      if ((theSprite->attribute0 & 0xff) > 0) {
	sprite_move(theSprite, 0, -1);
	bird->y = bird->y - 1; 
      }
    }
    else if ((theSprite->attribute0 & 0xff) < 148 && counter%2 == 0) {
      sprite_move(theSprite, 0, 1);
      bird->y = bird->y + 1;
    }

    sprite_move(coin, -1, 0);
    sprite_move(coin2, -1, 0);
    //Move Pipe 1
    sprite_move(sprite5, -1, 0);
    sprite_move(sprite6, -1, 0);
    sprite_move(sprite7, -1, 0);
    sprite_move(sprite8, -1, 0);
    sprite_move(sprite9, -1, 0);
    sprite_move(spritea, -1, 0);
    sprite_move(spriteb, -1, 0);
    sprite_move(spritec,-1,0);
    sprite_move(sprited,-1,0);
    sprite_move(sprite3, -1, 0);
    sprite_move(sprite4, -1, 0);
    sprite_move(sprite2, -1, 0);

    //Move Pipe 2
    sprite_move(s1prite, -1, 0);
    sprite_move(s2prite, -1, 0);
    sprite_move(s3prite, -1, 0);
    sprite_move(s4prite, -1, 0);
    sprite_move(s5prite, -1, 0);
    sprite_move(s6prite, -1, 0);
    sprite_move(s7prite, -1, 0);
    sprite_move(s8prite, -1, 0);
    sprite_move(s9prite, -1, 0);
    sprite_move(asprite, -1, 0);
    sprite_move(bsprite, -1, 0);
    sprite_move(csprite, -1, 0);
    sprite_move(dsprite, -1, 0);

    // convert theSprite x and y to local variables
    int spriteX = theSprite->attribute1 & 0x1ff;
    int spriteY = theSprite->attribute0 & 0xff;
    // get borders and determine conflict
    if (get_borders_and_determine_conflict(spriteX, spriteY, 1, sprite5->attribute1 & 0x1ff) == 1
        ||  get_borders_and_determine_conflict(spriteX, spriteY, 2, s1prite->attribute1 & 0x1ff) == 1) {
      *display_control = MODE0 | BG0_ENABLE | BG1_ENABLE | BG2_ENABLE;
      char score_str[100];
      char coin_str[100];
      sprintf(score_str, "you got %d points", scores->points);
      sprintf(coin_str, "you got %d coins", scores->coins);
      setup_endscreen(score_str, coin_str);
      game_ended = 1;
      game_in_progress = 0;
    }

    // set up score after pipe is passed
    if ((sprite5->attribute1 & 0x1ff+14 == spriteX+12) || (s1prite->attribute1 & 0x1ff+14 == spriteX+12)) {
      bird->touched_portal = 0;
      scores->points = track_score(bird->touched_portal, scores->points);
    } else {
        bird->touched_portal = 1;
        scores->points = track_score(bird->touched_portal, scores->points);
    }

    // test coin borders
    if (get_coin_borders_and_determine_conflict(spriteX, spriteY, 1, coin->attribute1 & 0x1ff) == 1
    || get_coin_borders_and_determine_conflict(spriteX, spriteY, 2, coin2->attribute1 & 0x1ff) == 1) {
	bird->coin_collected = 1;
	scores->coins = track_coins(bird->coin_collected, scores->coins);
    }

    //Calls a function to update the bird's position on-screen 
    bird_update(bird);
    //Increment counter.
    counter = counter + 1;
    //Checks if the counter is divisable by 10. If it is, moves 
    // the screen. I ued 10 to slow it down to look more natural.
    if (counter % 10 == 0) {
      xscroll++;
    }

    /* wait for vblank before scrolling */
    wait_vblank();
    //on_vblank();
    *bg0_x_scroll = xscroll;
    //*bg0_y_scroll = yscroll;
    *bg1_x_scroll = xscroll * 2;
    //*bg1_y_scroll = yscroll*2;
    if (counter == 1){
   //     currentBackground=0;
    }
    if (counter == 2){
     // currentBackground = 1;
    }
    else if (counter == 2000){
     // currentBackground = 2;
    }

    //relaods the background if you try to change it
    if (currentBackground != lastBackground && game_in_progress == 1) {
      setup_background(currentBackground);
    }
            
    //Updates where the sprites are located
    sprite_update_all();
    lastBackground = currentBackground;
    /* delay some */
    delay(50);
  }
}

