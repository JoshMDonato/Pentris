
/*

    =====================
    ===== PENTOMINO =====
    =====================

    A group of exactly 5 blocks.
    Each one falls through the board at a set speed.
    There are 18 different configurations:
    C, F, H, I, J, K, L, N, P, Q, S, T, U, V, W, X, Y, and Z.

    Specifically, this is a vector of pointers to blocks.
    The blocks actually belong to the board itself, or at least
     they ultimately will.
    This class will easily access things like where the current
     pentomino is, its orientation and configuration, etc.

*/

#include <vector>
#include <map>
#include <iostream>
#include <iterator>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#include <utilities.h>
#include <block.h>

#ifndef PENTOMINO_H
#define PENTOMINO_H

enum e_Name
{
    PENTOMINO_C,
    PENTOMINO_D,
    PENTOMINO_F,
    PENTOMINO_I,
    PENTOMINO_J,
    PENTOMINO_K,
    PENTOMINO_L,
    PENTOMINO_N,
    PENTOMINO_P,
    PENTOMINO_Q,
    PENTOMINO_S,
    PENTOMINO_T,
    PENTOMINO_U,
    PENTOMINO_V,
    PENTOMINO_W,
    PENTOMINO_X,
    PENTOMINO_Y,
    PENTOMINO_Z,
    PENTOMINO_NULL = 99
};

struct PENTOMINO_TINT
{
    unsigned char uc_Red;
    unsigned char uc_Green;
    unsigned char uc_Blue;
};

//Color scheme of each pentomino based on its configuration.
static std::map<unsigned char, PENTOMINO_TINT> m_Tint = {
    {PENTOMINO_C, {85,43,0}},
    {PENTOMINO_D, {170,85,255}},
    {PENTOMINO_F, {0,85,0}},
    {PENTOMINO_I, {85,255,170}},
    {PENTOMINO_J, {0,0,255}},
    {PENTOMINO_K, {85,0,0}},
    {PENTOMINO_L, {255,128,0}},
    {PENTOMINO_N, {255,0,0}},
    {PENTOMINO_P, {255,255,0}},
    {PENTOMINO_Q, {170,170,0}},
    {PENTOMINO_S, {0,128,255}},
    {PENTOMINO_T, {128,0,255}},
    {PENTOMINO_U, {0,255,0}},
    {PENTOMINO_V, {0,255,255}},
    {PENTOMINO_W, {255,0,128}},
    {PENTOMINO_X, {43,0,85}},
    {PENTOMINO_Y, {255,128,255}},
    {PENTOMINO_Z, {255,170,85}},
    {PENTOMINO_NULL,{30,30,30}}
};

struct PENTOMINO_INITIAL_POSITION
{
    int n;  //Block of the pentomino.
    int x;  //That block's initial x.
    int y;  //That block's initial y.
    int dx; //Horizontal movement on counterclockwise rotation.
    int dy; //Vertical movement on counterclockwise rotation.
};

//Initial position of each block of each type of pentomino. Initial orientation is always horizontal, flat side down.
static std::multimap<unsigned char, PENTOMINO_INITIAL_POSITION> m_InitialPosition = {
    {PENTOMINO_C, {0,0,0,  1, 1} }, //
    {PENTOMINO_C, {1,0,1,  2, 0} }, //   __    __
    {PENTOMINO_C, {2,1,1,  1,-1} }, //  |__|__|__|
    {PENTOMINO_C, {3,2,1,  0,-2} }, //  |__|__|__|
    {PENTOMINO_C, {4,2,0, -1,-1} }, //

    {PENTOMINO_D, {0,1,0,  0, 1} }, //  Because D sounds like T.
    {PENTOMINO_D, {1,0,1,  2, 1} }, //      __
    {PENTOMINO_D, {2,1,1,  1, 0} }, //   __|__|__ __
    {PENTOMINO_D, {3,2,1,  0,-1} }, //  |__|__|__|__|
    {PENTOMINO_D, {4,3,1, -1,-2} }, //

    {PENTOMINO_F, {0,1,0, -1, 1} }, //      __
    {PENTOMINO_F, {1,1,1,  0, 0} }, //     |__|__
    {PENTOMINO_F, {2,2,1, -1,-1} }, //   __|__|__|
    {PENTOMINO_F, {3,1,2,  1,-1} }, //  |__|__|
    {PENTOMINO_F, {4,0,2,  2, 0} }, //

    {PENTOMINO_I, {0,0,0,  2, 2} }, //
    {PENTOMINO_I, {1,1,0,  1, 1} }, //   __ __ __ __ __
    {PENTOMINO_I, {2,2,0,  0, 0} }, //  |__|__|__|__|__|
    {PENTOMINO_I, {3,3,0, -1,-1} }, //
    {PENTOMINO_I, {4,4,0, -2,-2} }, //

    {PENTOMINO_J, {0,0,0,  1, 2} }, //
    {PENTOMINO_J, {1,0,1,  2, 1} }, //   __
    {PENTOMINO_J, {2,1,1,  1, 0} }, //  |__|__ __ __
    {PENTOMINO_J, {3,2,1,  0,-1} }, //  |__|__|__|__|
    {PENTOMINO_J, {4,3,1, -1,-2} }, //

    {PENTOMINO_K, {0,1,0, -1, 1} }, //      __
    {PENTOMINO_K, {1,0,1,  1, 1} }, //   __|__|
    {PENTOMINO_K, {2,1,1,  0, 0} }, //  |__|__|__
    {PENTOMINO_K, {3,1,2,  1,-1} }, //     |__|__|
    {PENTOMINO_K, {4,2,2,  0,-2} }, //

    {PENTOMINO_L, {0,3,0, -2,-1} }, //
    {PENTOMINO_L, {1,0,1,  2, 1} }, //            __
    {PENTOMINO_L, {2,1,1,  1, 0} }, //   __ __ __|__|
    {PENTOMINO_L, {3,2,1,  0,-1} }, //  |__|__|__|__|
    {PENTOMINO_L, {4,3,1, -1,-2} }, //

    {PENTOMINO_N, {0,0,0,  1, 2} }, //
    {PENTOMINO_N, {1,1,0,  0, 1} }, //   __ __
    {PENTOMINO_N, {2,1,1,  1, 0} }, //  |__|__|__ __
    {PENTOMINO_N, {3,2,1,  0,-1} }, //     |__|__|__|
    {PENTOMINO_N, {4,3,1, -1,-2} }, //

    {PENTOMINO_P, {0,0,0,  1, 1} }, //
    {PENTOMINO_P, {1,1,0,  0, 0} }, //   __ __
    {PENTOMINO_P, {2,0,1,  2, 0} }, //  |__|__|__
    {PENTOMINO_P, {3,1,1,  1,-1} }, //  |__|__|__|
    {PENTOMINO_P, {4,2,1,  0,-2} }, //

    {PENTOMINO_Q, {0,1,0,  0, 0} }, //
    {PENTOMINO_Q, {1,2,0, -1,-1} }, //      __ __
    {PENTOMINO_Q, {2,0,1,  2, 0} }, //   __|__|__|
    {PENTOMINO_Q, {3,1,1,  1,-1} }, //  |__|__|__|
    {PENTOMINO_Q, {4,2,1,  0,-2} }, //

    {PENTOMINO_S, {0,1,0, -1, 1} }, //      __ __
    {PENTOMINO_S, {1,2,0, -2, 0} }, //     |__|__|
    {PENTOMINO_S, {2,1,1,  0, 0} }, //   __|__|
    {PENTOMINO_S, {3,0,2,  2, 0} }, //  |__|__|
    {PENTOMINO_S, {4,1,2,  1,-1} }, //

    {PENTOMINO_T, {0,1,0, -1, 1} }, //      __
    {PENTOMINO_T, {1,1,1,  0, 0} }, //     |__|
    {PENTOMINO_T, {2,0,2,  2, 0} }, //   __|__|__
    {PENTOMINO_T, {3,1,2,  1,-1} }, //  |__|__|__|
    {PENTOMINO_T, {4,2,2,  0,-2} }, //

    {PENTOMINO_U, {0,2,0, -1, 0} }, //  A lowercase U, kind of.
    {PENTOMINO_U, {1,3,0, -2,-1} }, //         __ __
    {PENTOMINO_U, {2,0,1,  2, 1} }, //   __ __|__|__|
    {PENTOMINO_U, {3,1,1,  1, 0} }, //  |__|__|__|
    {PENTOMINO_U, {4,2,1,  0,-1} }, //

    {PENTOMINO_V, {0,2,0, -2, 0} }, //         __
    {PENTOMINO_V, {1,2,1, -1,-1} }, //        |__|
    {PENTOMINO_V, {2,2,2,  0,-2} }, //   __ __|__|
    {PENTOMINO_V, {3,1,2,  1,-1} }, //  |__|__|__|
    {PENTOMINO_V, {4,0,2,  2, 0} }, //

    {PENTOMINO_W, {0,0,0,  0, 2} }, //   __
    {PENTOMINO_W, {1,0,1,  1, 1} }, //  |__|__
    {PENTOMINO_W, {2,1,1,  0, 0} }, //  |__|__|__
    {PENTOMINO_W, {3,1,2,  1,-1} }, //     |__|__|
    {PENTOMINO_W, {4,2,2,  0,-2} }, //

    {PENTOMINO_X, {0,1,0, -1, 1} }, //      __
    {PENTOMINO_X, {1,0,1,  1, 1} }, //   __|__|__
    {PENTOMINO_X, {2,1,1,  0, 0} }, //  |__|__|__|
    {PENTOMINO_X, {3,2,1, -1,-1} }, //     |__|
    {PENTOMINO_X, {4,1,2,  1,-1} }, //

    {PENTOMINO_Y, {0,2,0, -1, 0} }, //
    {PENTOMINO_Y, {1,0,1,  2, 1} }, //         __
    {PENTOMINO_Y, {2,1,1,  1, 0} }, //   __ __|__|__
    {PENTOMINO_Y, {3,2,1,  0,-1} }, //  |__|__|__|__|
    {PENTOMINO_Y, {4,3,1, -1,-2} }, //

    {PENTOMINO_Z, {0,0,0,  0, 2} }, //   __ __
    {PENTOMINO_Z, {1,1,0, -1, 1} }, //  |__|__|
    {PENTOMINO_Z, {2,1,1,  0, 0} }, //     |__|__
    {PENTOMINO_Z, {3,1,2,  1,-1} }, //     |__|__|
    {PENTOMINO_Z, {4,2,2,  0,-2} }  //
};

struct PENTOMINO_KICKOFF
{
    char kx1, ky1;
    char kx2, ky2;
    char kx3, ky3;
    char kx4, ky4;
};

//Table of kickoff tests based on pentomino configuration, as well as initial and final orientations.
static std::map<std::pair<char, std::pair<char,char>>, PENTOMINO_KICKOFF> m_Kickoffs = {
    {{PENTOMINO_C,{0,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_C,{1,0}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_C,{1,2}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_C,{2,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_C,{2,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },
    {{PENTOMINO_C,{3,2}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_C,{3,0}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_C,{0,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },

    {{PENTOMINO_D,{0,1}},   {-2,0,    1, 0,   -2, 1,     1,-2}   },
    {{PENTOMINO_D,{1,0}},   { 2,0,   -1, 0,    2,-1,    -1, 2}   },
    {{PENTOMINO_D,{1,2}},   {-1,0,    2, 0,   -1,-2,     2, 1}   },
    {{PENTOMINO_D,{2,1}},   { 1,0,   -2, 0,    1, 2,    -2,-1}   },
    {{PENTOMINO_D,{2,3}},   { 2,0,   -1, 0,    2,-1,    -1, 2}   },
    {{PENTOMINO_D,{3,2}},   {-2,0,    1, 0,   -2, 1,     1,-2}   },
    {{PENTOMINO_D,{3,0}},   { 1,0,   -2, 0,    1, 2,    -2,-1}   },
    {{PENTOMINO_D,{0,3}},   {-1,0,    2, 0,   -1,-2,     2, 1}   },

    {{PENTOMINO_F,{0,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_F,{1,0}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_F,{1,2}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_F,{2,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_F,{2,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },
    {{PENTOMINO_F,{3,2}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_F,{3,0}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_F,{0,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },

    {{PENTOMINO_I,{0,1}},   {1,-1,   -1, 1,    2,-2,    -2, 2}   },
    {{PENTOMINO_I,{1,0}},   {1,-1,   -1, 1,    2,-2,    -2, 2}   },
    {{PENTOMINO_I,{1,2}},   {1, 1,   -1,-1,    2, 2,    -2,-2}   },
    {{PENTOMINO_I,{2,1}},   {1, 1,   -1,-1,    2, 2,    -2,-2}   },
    {{PENTOMINO_I,{2,3}},   {1,-1,   -1, 1,    2,-2,    -2, 2}   },
    {{PENTOMINO_I,{3,2}},   {1,-1,   -1, 1,    2,-2,    -2, 2}   },
    {{PENTOMINO_I,{3,0}},   {1, 1,   -1,-1,    2, 2,    -2,-2}   },
    {{PENTOMINO_I,{0,3}},   {1, 1,   -1,-1,    2, 2,    -2,-2}   },

    {{PENTOMINO_J,{0,1}},   {-2,0,    1, 0,   -2, 1,     1,-2}   },
    {{PENTOMINO_J,{1,0}},   { 2,0,   -1, 0,    2,-1,    -1, 2}   },
    {{PENTOMINO_J,{1,2}},   {-1,0,    2, 0,   -1,-2,     2, 1}   },
    {{PENTOMINO_J,{2,1}},   { 1,0,   -2, 0,    1, 2,    -2,-1}   },
    {{PENTOMINO_J,{2,3}},   { 2,0,   -1, 0,    2,-1,    -1, 2}   },
    {{PENTOMINO_J,{3,2}},   {-2,0,    1, 0,   -2, 1,     1,-2}   },
    {{PENTOMINO_J,{3,0}},   { 1,0,   -2, 0,    1, 2,    -2,-1}   },
    {{PENTOMINO_J,{0,3}},   {-1,0,    2, 0,   -1,-2,     2, 1}   },

    {{PENTOMINO_K,{0,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_K,{1,0}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_K,{1,2}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_K,{2,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_K,{2,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },
    {{PENTOMINO_K,{3,2}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_K,{3,0}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_K,{0,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },

    {{PENTOMINO_L,{0,1}},   {-2,0,    1, 0,   -2, 1,     1,-2}   },
    {{PENTOMINO_L,{1,0}},   { 2,0,   -1, 0,    2,-1,    -1, 2}   },
    {{PENTOMINO_L,{1,2}},   {-1,0,    2, 0,   -1,-2,     2, 1}   },
    {{PENTOMINO_L,{2,1}},   { 1,0,   -2, 0,    1, 2,    -2,-1}   },
    {{PENTOMINO_L,{2,3}},   { 2,0,   -1, 0,    2,-1,    -1, 2}   },
    {{PENTOMINO_L,{3,2}},   {-2,0,    1, 0,   -2, 1,     1,-2}   },
    {{PENTOMINO_L,{3,0}},   { 1,0,   -2, 0,    1, 2,    -2,-1}   },
    {{PENTOMINO_L,{0,3}},   {-1,0,    2, 0,   -1,-2,     2, 1}   },

    {{PENTOMINO_N,{0,1}},   {-2,0,    1, 0,   -2, 1,     1,-2}   },
    {{PENTOMINO_N,{1,0}},   { 2,0,   -1, 0,    2,-1,    -1, 2}   },
    {{PENTOMINO_N,{1,2}},   {-1,0,    2, 0,   -1,-2,     2, 1}   },
    {{PENTOMINO_N,{2,1}},   { 1,0,   -2, 0,    1, 2,    -2,-1}   },
    {{PENTOMINO_N,{2,3}},   { 2,0,   -1, 0,    2,-1,    -1, 2}   },
    {{PENTOMINO_N,{3,2}},   {-2,0,    1, 0,   -2, 1,     1,-2}   },
    {{PENTOMINO_N,{3,0}},   { 1,0,   -2, 0,    1, 2,    -2,-1}   },
    {{PENTOMINO_N,{0,3}},   {-1,0,    2, 0,   -1,-2,     2, 1}   },

    {{PENTOMINO_P,{0,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_P,{1,0}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_P,{1,2}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_P,{2,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_P,{2,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },
    {{PENTOMINO_P,{3,2}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_P,{3,0}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_P,{0,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },

    {{PENTOMINO_Q,{0,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_Q,{1,0}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_Q,{1,2}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_Q,{2,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_Q,{2,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },
    {{PENTOMINO_Q,{3,2}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_Q,{3,0}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_Q,{0,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },

    {{PENTOMINO_S,{0,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_S,{1,0}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_S,{1,2}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_S,{2,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_S,{2,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },
    {{PENTOMINO_S,{3,2}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_S,{3,0}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_S,{0,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },

    {{PENTOMINO_T,{0,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_T,{1,0}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_T,{1,2}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_T,{2,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_T,{2,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },
    {{PENTOMINO_T,{3,2}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_T,{3,0}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_T,{0,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },

    {{PENTOMINO_U,{0,1}},   {-2,0,    1, 0,   -2, 1,     1,-2}   },
    {{PENTOMINO_U,{1,0}},   { 2,0,   -1, 0,    2,-1,    -1, 2}   },
    {{PENTOMINO_U,{1,2}},   {-1,0,    2, 0,   -1,-2,     2, 1}   },
    {{PENTOMINO_U,{2,1}},   { 1,0,   -2, 0,    1, 2,    -2,-1}   },
    {{PENTOMINO_U,{2,3}},   { 2,0,   -1, 0,    2,-1,    -1, 2}   },
    {{PENTOMINO_U,{3,2}},   {-2,0,    1, 0,   -2, 1,     1,-2}   },
    {{PENTOMINO_U,{3,0}},   { 1,0,   -2, 0,    1, 2,    -2,-1}   },
    {{PENTOMINO_U,{0,3}},   {-1,0,    2, 0,   -1,-2,     2, 1}   },

    {{PENTOMINO_V,{0,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_V,{1,0}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_V,{1,2}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_V,{2,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_V,{2,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },
    {{PENTOMINO_V,{3,2}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_V,{3,0}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_V,{0,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },

    {{PENTOMINO_W,{0,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_W,{1,0}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_W,{1,2}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_W,{2,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_W,{2,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },
    {{PENTOMINO_W,{3,2}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_W,{3,0}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_W,{0,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },

    {{PENTOMINO_X,{0,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_X,{1,0}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_X,{1,2}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_X,{2,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_X,{2,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },
    {{PENTOMINO_X,{3,2}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_X,{3,0}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_X,{0,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },

    {{PENTOMINO_Y,{0,1}},   {-2,0,    1, 0,   -2, 1,     1,-2}   },
    {{PENTOMINO_Y,{1,0}},   { 2,0,   -1, 0,    2,-1,    -1, 2}   },
    {{PENTOMINO_Y,{1,2}},   {-1,0,    2, 0,   -1,-2,     2, 1}   },
    {{PENTOMINO_Y,{2,1}},   { 1,0,   -2, 0,    1, 2,    -2,-1}   },
    {{PENTOMINO_Y,{2,3}},   { 2,0,   -1, 0,    2,-1,    -1, 2}   },
    {{PENTOMINO_Y,{3,2}},   {-2,0,    1, 0,   -2, 1,     1,-2}   },
    {{PENTOMINO_Y,{3,0}},   { 1,0,   -2, 0,    1, 2,    -2,-1}   },
    {{PENTOMINO_Y,{0,3}},   {-1,0,    2, 0,   -1,-2,     2, 1}   },

    {{PENTOMINO_Z,{0,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_Z,{1,0}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_Z,{1,2}},   { 1,0,    1, 1,    0,-2,     1,-2}   },
    {{PENTOMINO_Z,{2,1}},   {-1,0,   -1,-1,    0, 2,    -1, 2}   },
    {{PENTOMINO_Z,{2,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   },
    {{PENTOMINO_Z,{3,2}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_Z,{3,0}},   {-1,0,   -1, 1,    0,-2,    -1,-2}   },
    {{PENTOMINO_Z,{0,3}},   { 1,0,    1,-1,    0, 2,     1, 2}   }
};

class Pentomino
{
    public:
    Pentomino(unsigned char uc_type);                                   //Constructor
    ~Pentomino();                                                       //Destructor

    std::pair<int,int>  pullKickoffData(int i, int ri, int rf);         //Pull data off the kickoff table.


    //Gets
    unsigned char   getN()                      {return this->b_Blocks.size();}
    unsigned char   getType()                   {return this->uc_Type;}
    char            getOrientation()            {return this->c_Orientation;}
    Block*          getBlock(unsigned char n)   {return this->b_Blocks.at(n);}
    int             getShadow()                 {return this->i_Shadow;}

    //Sets
    void setN(unsigned char uc)                 {this->uc_N = uc;}
    void setType(unsigned char uc)              {this->uc_Type = uc;}
    void setOrientation(char c)                 {this->c_Orientation = c;}
    void setBlock(unsigned char n, Block* b)    {this->b_Blocks.at(n) = b;}
    void setShadow(int i)                       {this->i_Shadow = i;}


    private:
    unsigned char       uc_N                        = 5;                //Number of blocks (should normally be 5).
    unsigned char       uc_Type                     = PENTOMINO_NULL;   //Configuration of the pentomino.
    char                c_Orientation               = 0;                //Orientation (starts at 0). Counts -clockwise- rotations.
    std::vector<Block*> b_Blocks                    ;                   //The blocks.
    int                 i_Shadow                    = 0;                //Location of the shadow.

};

#endif //PENTOMINO_H
