//#include "player.h"
#include "flappin.h"
#include "background.h"
#include "background_night.h"
#include "background_dusk.h"
#include "map.h"
#include "map2.h"
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

  /* dest = char_block(0); */
  /* image = (unsigned short*) font_background_data; */
  /* for (int i = 0; i < ((font_background_width * font_background_height) / 2); i++) { */
  /*   dest[i] = font_background_data[i]; */
  /* } */

  *bg2_control = 0 |
    (0 << 2)  |
    (0 << 6)  |
    (1 << 7)  |
    (18 << 8) |
    (1 << 13) |
    (0 << 14);

  dest = screen_block(18);
  for (int i = 0; i < (numbers_width * numbers_height); i++) {
    dest[i] = numbers[i];
  }
}

void set_text(char* str, int row, int col) {
  /* find the index in the texmap to draw to */
  int index = row * 117 + col;
  
  int missing = 117;

  /* pointer to text map */
  volatile unsigned short* ptr = screen_block(18);

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
  setup_background(0);

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

void setup_endscreen() {
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
  // TODO re-enable bg2 when fix is found for score!!!
  *display_control = MODE0 | BG0_ENABLE | BG1_ENABLE | SPRITE_ENABLE | SPRITE_MAP_1D;
  /* store ints if game has started or is in progress or has ended. 0 = false, 1 = true */
  int game_started = 0;
  int game_in_progress = 1;
  int game_ended = 0;
	
  Scores scores = {
    .points = 0,
    .coins = 0,
  };
  /* setup the background 0 */
  setup_background(0);
  //Will save the value of the last background. 
  //Used to change if the background changes in-game.
  int lastBackground = 0;
  int currentBackground = 0;
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

  int spriteMode = 0;
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
      setup_endscreen();
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
      setup_endscreen();
      game_ended = 1;
      game_in_progress = 0;
    }

    // set up score after pipe is passed
    int score = 0;
    if ((sprite5->attribute1 & 0x1ff+14 == spriteX+12) || (s1prite->attribute1 & 0x1ff+14 == spriteX+12)) {
      bird->touched_portal = 1;
    }

    int touched_portal = bird->touched_portal;
    track_score(touched_portal, scores.points);
    char* score_str;
    sprintf(score_str, "%d", scores.points);
    set_text(score_str, WIDTH/2, 40);
    bird->touched_portal = 0;

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
    *bg0_x_scroll = xscroll;
    //*bg0_y_scroll = yscroll;
    *bg1_x_scroll = xscroll * 2;
    //*bg1_y_scroll = yscroll*2;
    if (counter == 1000){
      currentBackground = 1;
    }
    else if (counter == 2000){
      currentBackground = 2;
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
