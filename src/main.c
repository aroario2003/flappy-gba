/* include the tile maps we are using for back and fore ground */
//#include "player.h"
#include "flappin.h"
#include "background.h"
#include "background_night.h"
#include "background_dusk.h"
#include "map.h"
#include "map2.h"
// SCREEN_WIDTH and SCREEN_HEIGHT defined here (below)
#include "game.h"
#include "sprites.h"
#include "font.h"
//#include "fb_song.h"
#include "bird.h"
#include "scores.h"
#include "starting_screen.h"
// #include "starting_screen2.h"
#include "sprite.c"
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
#define WIDTH 240
#define HEIGHT 160

/* the control registers for the four tile layers */
volatile unsigned short* screen = (volatile unsigned short*) 0x6000000;
volatile unsigned short* bg0_control = (volatile unsigned short*) 0x4000008;
volatile unsigned short* bg1_control = (volatile unsigned short*) 0x400000a;
volatile unsigned short* bg2_control = (volatile unsigned short*) 0x400000c;
volatile unsigned short* bg3_control = (volatile unsigned short*) 0x400000e;

/* palette is always 256 colors */
#define PALETTE_SIZE 256

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
            bg_palette[i] = background_palette[i];
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
}

/* set up game started in background 2 */
void setup_game_started() {

    volatile unsigned short* dest = char_block(0);

    /* works but not as intended */
    unsigned short* image = (unsigned short*) background_data;
    for (int i = 0; i < ((background_width * background_height) / 2); i++) {
        dest[i] = image[i];
    }

    /* does not work
    unsigned short* image = (unsigned short*) starting_screen2_data;
    for (int i = 0; i < ((starting_screen2_width * starting_screen2_height) / 2); i++) {
        dest[i] = image[i];
    }
     */


    /* load palette supporting background and starting screen */
    for (int i = 0; i < PALETTE_SIZE; i++) {
        bg_palette[i] = starting_screen_and_background_palette[i];
    }

  //  *bg0_control = 2 |    /* priority, 0 is highest, 3 is lowest */
                   (0 << 2)  |       /* the char block the image data is stored in */
                   (0 << 6)  |       /* the mosaic flag */
                   (1 << 7)  |       /* color mode, 0 is 16 colors, 1 is 256 colors */
                   (16 << 8) |       /* the screen block the tile data is stored in */
                   (1 << 13) |       /* wrapping flag */
                   (0 << 14);        /* bg size, 0 is 256x256 */

//    *bg1_control = 1 |
                   (0 << 2)  |
                   (0 << 6)  |
                   (1 << 7)  |
                   (17 << 8) |
                   (1 << 13) |
                   (0 << 14);

    *bg2_control = 0 |
                   (0 << 2)  |
                   (0 << 6)  |
                   (1 << 7)  |
                   (18 << 8) |
                   (1 << 13) |
                   (0 << 14);

    /* load the tile data into screen block 18 */
    dest = screen_block(18);
    for (int i = 0; i < (starting_screen_width * starting_screen_height); i++) {
        dest[i] = starting_screen[i];
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

/* the main function */
int main() {

    *display_control = MODE3 | BG2;
    for (int row = 0; row < HEIGHT; row++) {
        for (int col = 0; col < WIDTH; col++) {
            put_pixel_m3(row, col, flappin_data[row * WIDTH + col]);
        }
    }

    handle_start();
    /* we set the mode to mode 0 with bg0 on */
    *display_control = MODE0 | BG0_ENABLE | BG1_ENABLE | BG2_ENABLE;    
    /* store ints if game has started or is in progress or has ended. 0 = false, 1 = true */
    int game_started = 1;
    int game_in_progress = 0;
    int game_ended = 0;
    /* setup the background 0 */
    setup_background(0);
    setup_game_started();
    
    //Will save the value of the last background. Used to change if the background changes in-game.
    int lastBackground=0;
    int currentBackground=0;
    /* set initial scroll to 0 */
    int xscroll = 0;
    int yscroll = 0;
     setup_sprite_image();
     sprite_clear();

    Bird bird;
    bird_init(&bird);
    //Counter. Counts the amount of ticks we have gone along.
    int counter = 0;
    /* loop forever */
    while (1) {
        //Increment counter.
        counter = counter+1;

        //Checks if the counter is divisable by 10. If it is, moves the screen. I ued 10 to slow it down to look more natural.

        if (counter%10==0){
            xscroll++;
        }

        /* wait for vblank before scrolling */
        wait_vblank();
        *bg0_x_scroll = xscroll;
        //*bg0_y_scroll = yscroll;
        *bg1_x_scroll = xscroll*2;
        //*bg1_y_scroll = yscroll*2;

        
        //relaods the background if you try to change it
        if (currentBackground != lastBackground){
            setup_background(currentBackground);
        } 
        lastBackground = currentBackground;
        /* delay some */
        delay(50);
    }
}

