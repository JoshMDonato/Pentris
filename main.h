
/*

    ================
    ===== MAIN =====
    ================

    The main loop of the program.
    The loop is essentially three steps; receive input, process input,
     and update the display.
    The program also starts and stops here.

*/

#include <allegro5/allegro.h>                               //Allegro
#include <allegro5/allegro_image.h>                         //Images
#include <allegro5/allegro_native_dialog.h>                 //Dialogue Boxes
#include <allegro5/allegro_font.h>                          //Fonts
#include <allegro5/allegro_ttf.h>                           //TrueType Fonts
#include <allegro5/allegro_audio.h>                         //Audio
#include <allegro5/allegro_acodec.h>                        //Audio Codecs
#include <allegro5/allegro_physfs.h>                        //PhysFS interface

#include <physfs.h>                                         //PhysFS

#include <random>
#include <algorithm>
#include <chrono>

#include <utilities.h>
#include <board.h>
#include <preview.h>
#include <hold.h>

#ifndef LOOP_H
#define LOOP_H


//Seed the (global) RNG.
std::mt19937_64                                     mt_generator(std::chrono::high_resolution_clock::now().time_since_epoch().count());
std::uniform_int_distribution<unsigned char>        uid(0,17);

//Handles for each sound effect.
enum e_SFX
{
    SFX_START_TIMER,
    SFX_MOVE,
    SFX_ROTATE,
    SFX_LOCK,
    SFX_HOLD,
    SFX_CLEAR_SINGLE,
    SFX_CLEAR_DOUBLE,
    SFX_CLEAR_TRIPLE,
    SFX_CLEAR_QUADRUPLE,
    SFX_CLEAR_PENTRIS,
    SFX_PAUSE,
    SFX_UNPAUSE,
    SFX_NOMOVE,
    SFX_KILLBOARD,
    SFX_NULL,
};

class MainLoop
{
    public:

    MainLoop();                                             //Constructor
    ~MainLoop();                                            //Destructor

    void                        Frame();                    //Process Frame
    void                        newGame();                  //Set up a new game.
    void                        die();                      //Die :/
    void                        unpause();                  //Unpause.
    void                        drawPentomino();            //Set the order of pentominoes to come.
    void                        trySoftDrop();              //Try to soft drop.
    void                        tryHardDrop();              //Try to hard drop.
    void                        trySpawn();                 //Try to spawn a new pentomino.
    void                        hold();                     //Store the current pentomino in the hold, and spawn the pentomino from the hold if there is one.

    //Gets
    bool                        getKey(unsigned int ui)     {return this->b_Keys[ui];}
    bool                        getKillSwitch()             {return this->b_Kill;}
    bool                        getDrawSwitch()             {return this->b_Draw;}
    ALLEGRO_DISPLAY*            getDisplay()                {return this->d_Screen;}
    ALLEGRO_SAMPLE*             getBGM()                    {return this->s_BGM;}
    ALLEGRO_SAMPLE*             getSFX(int i)               {return this->s_SFX[i];}
    ALLEGRO_TIMER*              getTime()                   {return this->t_Time;}
    ALLEGRO_EVENT_QUEUE*        getQueue()                  {return this->q_Events;}
    ALLEGRO_FONT*               getFont()                   {return this->f_Font;}
    Board*                      getBoard()                  {return this->b_Board;}
    Preview*                    getPreview()                {return this->p_Preview;}
    Hold*                       getHold()                   {return this->h_Hold;}
    unsigned char               getMusic()                  {return this->uc_Music;}
    unsigned char               getHolds()                  {return this->uc_Hold;}
    std::vector<unsigned char>& getOrder()                  {return this->uc_Order;}
    unsigned char               getBag(int i)               {return this->uc_Bag[i];}
    unsigned char               getBagsize()                {return this->uc_Bagsize;}
    ALLEGRO_TIMER*              getSoftDropTimer()          {return this->t_SoftDrop;}
    ALLEGRO_TIMER*              getAutoShiftTimer()         {return this->t_DAS;}
    ALLEGRO_TIMER*              getLockDelayTimer()         {return this->t_LockDelay;}
    ALLEGRO_TIMER*              getUnpauseTimer()           {return this->t_UnpauseDelay;}
    ALLEGRO_TIMER*              getGameOverTimer()          {return this->t_GameOver;}
    unsigned char               getPaused()                 {return this->uc_Paused;}
    bool                        getGameOver()               {return this->b_Dead;}

    //Sets
    void    setKey(unsigned int ui, bool b)                 {this->b_Keys[ui] = b;}
    void    setKillSwitch(bool b)                           {this->b_Kill = b;}
    void    setDrawSwitch(bool b)                           {this->b_Draw = b;}
    void    setDisplay(ALLEGRO_DISPLAY* d)                  {this->d_Screen = d;}
    void    setBGM(ALLEGRO_SAMPLE* s)                       {this->s_BGM = s;}
    void    setSFX(int i, ALLEGRO_SAMPLE* s)                {this->s_SFX[i] = s;}
    void    setTime(ALLEGRO_TIMER* t)                       {this->t_Time = t;}
    void    setQueue(ALLEGRO_EVENT_QUEUE* q)                {this->q_Events = q;}
    void    setFont(ALLEGRO_FONT* f)                        {this->f_Font = f;}
    void    setBoard(Board* b)                              {this->b_Board = b;}
    void    setPreview(Preview* p)                          {this->p_Preview = p;}
    void    setHold(Hold* h)                                {this->h_Hold = h;}
    void    setMusic(unsigned char uc)                      {this->uc_Music = uc;}
    void    setHolds(unsigned char uc)                      {this->uc_Hold = uc;}
    void    setOrder(std::vector<unsigned char> v)          {this->uc_Order = v;}
    void    setBag(int i, unsigned char uc)                 {this->uc_Bag[i] = uc;}
    void    setBagsize(unsigned char uc)                    {this->uc_Bagsize = uc;}
    void    setSoftDropTimer(ALLEGRO_TIMER* t)              {this->t_SoftDrop = t;}
    void    setAutoShiftTimer(ALLEGRO_TIMER* t)             {this->t_DAS = t;}
    void    setLockDelayTimer(ALLEGRO_TIMER* t)             {this->t_LockDelay = t;}
    void    setUnpauseTimer(ALLEGRO_TIMER* t)               {this->t_UnpauseDelay = t;}
    void    setGameOverTimer(ALLEGRO_TIMER* t)              {this->t_GameOver = t;}
    void    setPaused(unsigned char uc)                     {this->uc_Paused = uc;}
    void    setGameOver(bool b)                             {this->b_Dead = b;}


    //Variables
    private:
    bool                        b_Keys[ALLEGRO_KEY_MAX]     ;                       //Tracks held keys as needed.
    bool                        b_Kill                      = false;                //Program closed?
    bool                        b_Draw                      = true;                 //Draw to screen?
    ALLEGRO_DISPLAY*            d_Screen                    = nullptr;              //The screen itself.
    ALLEGRO_SAMPLE*             s_BGM                       = nullptr;              //BGM
    ALLEGRO_SAMPLE*             s_SFX[SFX_NULL]             ;                       //Array of all sound effects.
    ALLEGRO_TIMER*              t_Time                      = nullptr;              //Age of program in frames.
    ALLEGRO_EVENT_QUEUE*        q_Events                    = nullptr;              //Event queue.
    ALLEGRO_FONT*               f_Font                      = nullptr;              //Font
    Board*                      b_Board                     = nullptr;              //The board.
    Preview*                    p_Preview                   = nullptr;              //The preview window.
    Hold*                       h_Hold                      = nullptr;              //The hold window.
    unsigned char               uc_Music                    = 0;                    //Current BGM
    unsigned char               uc_Hold                     = UTIL_HOLDS_PER_TURN;  //Number of holds remaining this turn.
    std::vector<unsigned char>  uc_Order                    ;                       //Order of pentominoes to come.
    std::array<unsigned char,7> uc_Bag                      ;                       //The bag holding the next 7 pentominoes.
    unsigned char               uc_Bagsize                  = 0;                    //Remaining pentominoes in the bag.
    ALLEGRO_TIMER*              t_SoftDrop                  = nullptr;              //Global timer to govern automatic soft drops.
    ALLEGRO_TIMER*              t_DAS                       = nullptr;              //Global timer to govern delayed auto shift.
    ALLEGRO_TIMER*              t_LockDelay                 = nullptr;              //Global timer to govern lock delay on grounded pentominoes.
    ALLEGRO_TIMER*              t_UnpauseDelay              = nullptr;              //Global timer to govern unpausing or starting a new game.
    ALLEGRO_TIMER*              t_GameOver                  = nullptr;              //Global timer to animate the game over screen.
    unsigned char               uc_Paused                   = UTIL_UNPAUSE_TIME+1;  //Seconds to unpause (0 if unpaused).
    bool                        b_Dead                      = false;                //Game over?

};

#endif //LOOP_H


