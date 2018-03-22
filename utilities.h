
/*

    =====================
    ===== UTILITIES =====
    =====================

    Various definitions and commonly used functions are here.

*/

#include <math.h>
#include <random>


#ifndef UTIL_H
#define UTIL_H


#define UTIL_FPS                60                                      //Frames per second.
#define UTIL_FREQUENCY          0.0167                                  //Should be 1/UTIL_FPS.

#define UTIL_BLOCK_SIZE         24                                      //Block size.

#define UTIL_GRID_WIDTH         13                                      //Board width in blocks. Needs to be more than 5.
#define UTIL_GRID_HEIGHT        (2*UTIL_GRID_WIDTH+1)                   //Board height in blocks. Needs to be more than 5.

#define UTIL_FRAME_THICKNESS    UTIL_BLOCK_SIZE/2                       //Thickness of the frames bounding each section.

#define UTIL_BOARD_WIDTH        UTIL_BLOCK_SIZE*UTIL_GRID_WIDTH         //Board width in pixels.
#define UTIL_BOARD_HEIGHT       UTIL_BLOCK_SIZE*UTIL_GRID_HEIGHT        //Board height in pixels.

#define UTIL_PREVIEW_WIDTH      5*UTIL_BLOCK_SIZE                       //Preview width in pixels.
#define UTIL_PREVIEW_HEIGHT     4*UTIL_BLOCK_SIZE*UTIL_PREVIEW_NUMBER   //Preview height in pixels.

#define UTIL_HOLD_WIDTH         UTIL_PREVIEW_WIDTH                      //Hold width in pixels.
#define UTIL_HOLD_HEIGHT        4*UTIL_BLOCK_SIZE*UTIL_HOLD_NUMBER      //Hold height in pixels.

#define UTIL_BOARD_X            8*UTIL_BLOCK_SIZE                       //Upper left corner of the board on the screen. Should be far enough over to show the hold window.
#define UTIL_BOARD_Y            2*UTIL_BLOCK_SIZE                       //Upper left corner of the board on the screen.

#define UTIL_PREVIEW_X          UTIL_BOARD_X + UTIL_BOARD_WIDTH         //Upper left corner of the preview on the screen.
#define UTIL_PREVIEW_Y          UTIL_BOARD_Y                            //Upper left corner of the preview on the screen.

#define UTIL_HOLD_X             UTIL_BOARD_X - UTIL_HOLD_WIDTH          //Upper left corner of the hold.
#define UTIL_HOLD_Y             UTIL_BOARD_Y                            //Upper left corner of the hold.

#define UTIL_SCREEN_WIDTH       UTIL_PREVIEW_X + UTIL_PREVIEW_WIDTH + 13*UTIL_BLOCK_SIZE  //Screen width in pixels.
#define UTIL_SCREEN_HEIGHT      UTIL_BLOCK_SIZE*(4+UTIL_GRID_HEIGHT)    //Screen height in pixels.

#define UTIL_BGM_NUMBER         3                                       //Number of available BGMs to choose from.
#define UTIL_BGM_VOLUME         0.9                                     //BGM volume.
#define UTIL_SFX_VOLUME         0.75                                    //SFX volume.

#define UTIL_INITIAL_X          (int)floor((UTIL_GRID_WIDTH-1)/2)-1     //The leftmost block of each pentomino spawns in this column.
#define UTIL_INITIAL_Y          0                                       //The topmost block of each pentomino spawns in this row.

#define UTIL_PREVIEW_NUMBER     6                                       //Number of pentominoes to preview. Should be between 1 and 6.
#define UTIL_HOLD_NUMBER        1                                       //Number of pentominoes which can be held. Normally 1 but may increase to decrease difficulty.

#define UTIL_LOCK_DELAY         0.5                                     //Time a pentomino can spend touching something below it before it locks.
#define UTIL_AUTO_SHIFT_DELAY   0.25                                    //Time left or right can be held before a pentomino slides faster.
#define UTIL_FAST_GRAVITY       0.05                                    //Speed of the pentomino when a directional key is held down after any delays.

#define UTIL_HOLDS_PER_TURN     UTIL_HOLD_NUMBER                        //How many times the player can hold per turn.

#define UTIL_UNPAUSE_TIME       3                                       //Seconds to unpause.

#define UTIL_GRAVITY_ALPHA      0.8                                     //Parameters for determining pentomino speed.
#define UTIL_GRAVITY_BETA       0.0035                                  //Smaller alpha or larger beta increase overall speed.

#endif //UTIL_H
