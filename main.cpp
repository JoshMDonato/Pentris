
/*

    ===================
    ===== PENTRIS =====
    ===================

    In this game, you control a PENTOMINO as it falls through the BOARD.
    A PENTOMINO is comprised of five BLOCKS.
    The objective is to maneuver the PENTOMINOES so that they form a horizontal
     line of BLOCKS, clearing them off the BOARD.
    PENTOMINOES can be moved left, right, down, as well as rotated 90 degrees
     clockwise or counterclockwise.
    As more lines are cleared, the speed of the PENTOMINOES will increase.
    Game is over once it's impossible to spawn a new PENTOMINO in empty space.

    Mechanics are based on the Tetris Guideline as much as possible.
    https://tetris.wiki/Tetris_Guideline

*/

#include <main.h>

//////////////////////////////////////////////////
int main(int argc, char **argv)
{
    //Start the loop.
    MainLoop* L = new MainLoop();

    //Do it!
    while(!L->getKillSwitch())
    {
        L->Frame();
    }

    //Stop the loop.
    L->~MainLoop();
    return 0;
}
//////////////////////////////////////////////////


//Constructor
MainLoop::MainLoop()
{
    srand(time(nullptr));
    this->setKillSwitch(false);

    //Start Allegro.
    if(!al_init())
    {
        al_show_native_message_box(this->getDisplay(), "Error","Error","Can't load Allegro!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        this->setKillSwitch(true);
    }
    //Abort bootup entirely if Allegro can't start.
    else
    {
        //Start and setup PhysFS
        PHYSFS_init(nullptr);
        PHYSFS_addToSearchPath("Pentris.dat",1);
        al_set_physfs_file_interface();

        //Start the image addon.
        if(!al_init_image_addon())
        {
            al_show_native_message_box(this->getDisplay(), "Error","Error","Can't load image addon!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
            this->setKillSwitch(true);
        }

        //Start the font addon.
        if(!al_init_font_addon())
        {
            al_show_native_message_box(this->getDisplay(), "Error","Error","Can't load font addon!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
            this->setKillSwitch(true);
        }

        //Start the font addon.
        if(!al_init_ttf_addon())
        {
            al_show_native_message_box(this->getDisplay(), "Error","Error","Can't load TTF addon!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
            this->setKillSwitch(true);
        }

        //Start the audio addon.
        if(!al_install_audio())
        {
            al_show_native_message_box(this->getDisplay(), "Error","Error","Can't load audio addon!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            this->setKillSwitch(true);
        }

        //Register audio codecs.
        if(!al_init_acodec_addon())
        {
            al_show_native_message_box(this->getDisplay(), "Error","Error","Can't load audio codecs!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            this->setKillSwitch(true);
        }

        //Reserve enough samples for all sound effects plus BGM.
        if(!al_reserve_samples(SFX_NULL))
        {
            al_show_native_message_box(this->getDisplay(), "Error","Error","Can't reserve samples!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            this->setKillSwitch(true);
        }

        //Load the audio (externally for now).
        this->setBGM(al_load_sample("type-A.ogg"));
        this->setSFX(SFX_START_TIMER,al_load_sample("sfx_timer.wav"));
        this->setSFX(SFX_MOVE,al_load_sample("sfx_move.wav"));
        this->setSFX(SFX_ROTATE,al_load_sample("sfx_rotate.wav"));
        this->setSFX(SFX_LOCK,al_load_sample("sfx_lock.wav"));
        this->setSFX(SFX_HOLD,al_load_sample("sfx_hold.wav"));
        this->setSFX(SFX_CLEAR_SINGLE,al_load_sample("sfx_clear_1.wav"));
        this->setSFX(SFX_CLEAR_DOUBLE,al_load_sample("sfx_clear_2.wav"));
        this->setSFX(SFX_CLEAR_TRIPLE,al_load_sample("sfx_clear_3.wav"));
        this->setSFX(SFX_CLEAR_QUADRUPLE,al_load_sample("sfx_clear_4.wav"));
        this->setSFX(SFX_CLEAR_PENTRIS,al_load_sample("sfx_clear_5.wav"));
        this->setSFX(SFX_PAUSE,al_load_sample("sfx_pause.wav"));
        this->setSFX(SFX_UNPAUSE,al_load_sample("sfx_unpause.wav"));
        this->setSFX(SFX_NOMOVE,al_load_sample("sfx_nomove.wav"));
        this->setSFX(SFX_KILLBOARD,al_load_sample("sfx_killboard.wav"));

        //Start the display.
        this->setDisplay(al_create_display(UTIL_SCREEN_WIDTH,UTIL_SCREEN_HEIGHT));
        if(!this->getDisplay())
        {
            al_show_native_message_box(this->getDisplay(), "Error","Error","Can't load screen!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
            this->setKillSwitch(true);
        }

        //Load the font.
        this->setFont(al_load_ttf_font("Flipbash.ttf",UTIL_BLOCK_SIZE,0));

        //Detect the keyboard.
        if(!al_install_keyboard())
        {
            al_show_native_message_box(this->getDisplay(), "Error","Error","Can't initialize keyboard!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
            this->setKillSwitch(true);
        }

        //Setup and start timers.
        this->setTime(al_create_timer(UTIL_FREQUENCY)); //Locks frame rate.
        this->setSoftDropTimer(al_create_timer(0.5));
        this->setAutoShiftTimer(al_create_timer(UTIL_AUTO_SHIFT_DELAY));
        this->setLockDelayTimer(al_create_timer(UTIL_LOCK_DELAY));
        this->setUnpauseTimer(al_create_timer(1.0));
        this->setGameOverTimer(al_create_timer(0.05));
        al_start_timer(this->getTime());
        al_start_timer(this->getSoftDropTimer());

        //Start the event queue.
        this->setQueue(al_create_event_queue());

        //Allow events related to the display.
        al_register_event_source(this->getQueue(), al_get_display_event_source(this->getDisplay()));

        //Allow events related to the mouse.
        //al_register_event_source(this->getQueue(), al_get_mouse_event_source());

        //Allow events related to the keyboard.
        al_register_event_source(this->getQueue(), al_get_keyboard_event_source());

        //Allow events related to the timers.
        al_register_event_source(this->getQueue(),al_get_timer_event_source(this->getSoftDropTimer()));
        al_register_event_source(this->getQueue(),al_get_timer_event_source(this->getAutoShiftTimer()));
        al_register_event_source(this->getQueue(),al_get_timer_event_source(this->getLockDelayTimer()));
        al_register_event_source(this->getQueue(),al_get_timer_event_source(this->getUnpauseTimer()));
        al_register_event_source(this->getQueue(),al_get_timer_event_source(this->getGameOverTimer()));

        //Start a new game.
        this->newGame();
    }
}

//Destructor
MainLoop::~MainLoop()
{
    //Unload the board.
    delete this->getBoard();

    //Unload the preview.
    delete this->getPreview();

    //Unload the hold.
    delete this->getHold();

    //Unload the event queue.
    if(this->getQueue())
    {
        al_destroy_event_queue(this->getQueue());
    }

    //Unload the font.
    if(this->getFont())
    {
        al_destroy_font(this->getFont());
    }

    //Destroy the audio.
    if(this->getBGM())
    {
        al_destroy_sample(this->getBGM());
    }
    for(int i = 0; i < SFX_NULL; i++)
    {
        if(this->getSFX(i))
        {
            al_destroy_sample(this->getSFX(i));
        }
    }

    //Close the display.
    if(this->getDisplay())
    {
        al_destroy_display(this->getDisplay());
    }

    //Destroy all timers.
    if(this->getTime())
    {
        al_destroy_timer(this->getTime());
    }
    if(this->getSoftDropTimer())
    {
        al_destroy_timer(this->getSoftDropTimer());
    }
    if(this->getAutoShiftTimer())
    {
        al_destroy_timer(this->getAutoShiftTimer());
    }
    if(this->getUnpauseTimer())
    {
        al_destroy_timer(this->getUnpauseTimer());
    }
    if(this->getGameOverTimer())
    {
        al_destroy_timer(this->getGameOverTimer());
    }

    //Uninstall the keyboard.
    al_uninstall_keyboard();

    //Stop PhysFS
    PHYSFS_deinit();

    //Stop Allegro and all its addons.
    al_uninstall_system();
}


//Process one frame.
void MainLoop::Frame()
{
    //Listen for events occurring this frame.
    ALLEGRO_EVENT e;
    ALLEGRO_TIMEOUT to;
    al_init_timeout(&to, UTIL_FREQUENCY);
    bool w = al_wait_for_event_until(this->getQueue(), &e, &to); //Hold for the duration of one frame.

    //React to any events that occurred. TODO: Make sure -ALL- events are processed this frame.
    //while(!al_is_event_queue_empty(this->getQueue()))
    if(w)
    {
        //At least one event occurred. Draw this frame.
        this->setDrawSwitch(true);

        if(e.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            //Window closed
            this->setKillSwitch(true);
            return;
        }

        //Automatic event occurring.
        if(e.type == ALLEGRO_EVENT_TIMER)
        {
            //Game over timer ticked. Animate kill screen.
            if(e.timer.source == this->getGameOverTimer())
            {
                if(this->getBoard()->killBoard())
                {
                    al_play_sample(this->getSFX(SFX_KILLBOARD),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);
                }
            }
            //Unpause timer ticked. Start to resume game.
            if(e.timer.source == this->getUnpauseTimer())
            {
                this->unpause();
            }
            //Soft drop timer ticked. If game is live, try to soft drop.
            if(e.timer.source == this->getSoftDropTimer() && (!this->getGameOver() && !this->getPaused()))
            {
                this->trySoftDrop();
            }
            //DAS timer ticked. Only active while game is live.
            if((e.timer.source == this->getAutoShiftTimer()) && (!this->getGameOver() && !this->getPaused()))
            {
                //Left key held. Try to move left.
                if(this->getKey(ALLEGRO_KEY_LEFT))
                {
                    this->getBoard()->moveLeft();
                    al_set_timer_speed(this->getAutoShiftTimer(),UTIL_FAST_GRAVITY);
                }
                //Right key held. Try to move right.
                else if(this->getKey(ALLEGRO_KEY_RIGHT))
                {

                    this->getBoard()->moveRight();
                    al_set_timer_speed(this->getAutoShiftTimer(),UTIL_FAST_GRAVITY);
                }
            }
            //Lock Delay timer ticked. Only active while game is live.
            if(e.timer.source == this->getLockDelayTimer() && (!this->getGameOver() && !this->getPaused()))
            {
                //Try one more time to soft drop.
                this->trySoftDrop();
            }
        }

        //Key pressed. Controls active at any time.
        if(e.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            int k = e.keyboard.keycode;
            this->setKey(k,true);

            //N key pressed.
            if(k == ALLEGRO_KEY_N)
            {
                //Start a new game.
                this->newGame();
            }

            //P key pressed.
            if(k == ALLEGRO_KEY_P && !this->getGameOver())
            {
                //Start to unpause if paused.
                if(this->getPaused() == UTIL_UNPAUSE_TIME+1)
                {
                    al_start_timer(this->getUnpauseTimer());
                    this->unpause();
                }
                //Pause if unpaused.
                if(!this->getPaused())
                {
                    this->setPaused(UTIL_UNPAUSE_TIME+1);
                    al_stop_samples();
                    al_play_sample(this->getSFX(SFX_PAUSE),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);
                }
            }
            //M key pressed.
            if(k == ALLEGRO_KEY_M)
            {
                //Change music.
                this->setMusic((this->getMusic()+1)%UTIL_BGM_NUMBER);
                switch(this->getMusic())
                {
                    case 0:             this->setBGM(al_load_sample("type-A.ogg")); break;
                    case 1:             this->setBGM(al_load_sample("type-B.ogg")); break;
                    case 2: default:    this->setBGM(al_load_sample("type-C.ogg")); break;
                }
                //If game is live, switch to the new music right now.
                if(!this->getGameOver() && !this->getPaused())
                {
                    al_stop_samples();
                    al_play_sample(this->getBGM(),UTIL_BGM_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_LOOP,nullptr);
                }
            }
        }
        //Key pressed. Controls only active while game is live.
        if(e.type == ALLEGRO_EVENT_KEY_DOWN && (!this->getGameOver() && !this->getPaused()))
        {
            int k = e.keyboard.keycode;
            this->setKey(k,true);
            //Left key pressed. Takes priority over right key.
            if(k == ALLEGRO_KEY_LEFT)
            {
                //Try to move the current pentomino left.
                if(this->getBoard()->moveLeft())
                {
                    al_play_sample(this->getSFX(SFX_MOVE),UTIL_SFX_VOLUME-0.3,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);
                }
                else
                {
                    al_play_sample(this->getSFX(SFX_NOMOVE),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);
                }
                al_start_timer(this->getAutoShiftTimer());
            }
            //Right key pressed.
            else if(k == ALLEGRO_KEY_RIGHT)
            {
                //Try to move the current pentomino right.
                if(this->getBoard()->moveRight())
                {
                    al_play_sample(this->getSFX(SFX_MOVE),UTIL_SFX_VOLUME-0.3,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);
                }
                else
                {
                    al_play_sample(this->getSFX(SFX_NOMOVE),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);
                }
                al_start_timer(this->getAutoShiftTimer());
            }
            //Down key pressed. Can be combined with left or right.
            if(k == ALLEGRO_KEY_DOWN)
            {
                //Accelerate the pentomino for the duration of the key press.
                al_stop_timer(this->getSoftDropTimer());
                al_set_timer_speed(this->getSoftDropTimer(),UTIL_FAST_GRAVITY);
                al_start_timer(this->getSoftDropTimer());
            }
            //Spacebar pressed.
            if(k == ALLEGRO_KEY_SPACE)
            {
                //Hard drop.
                this->tryHardDrop();
            }
            //Z or Ctrl key pressed. Takes priority over X or up.
            if(k == ALLEGRO_KEYMOD_CTRL || k == ALLEGRO_KEY_Z)
            {
                //Try to rotate the pentomino counterclockwise.
                if(this->getBoard()->rotateLeft())
                {
                    al_play_sample(this->getSFX(SFX_ROTATE),UTIL_SFX_VOLUME-0.3,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);
                }
                else
                {
                    al_play_sample(this->getSFX(SFX_NOMOVE),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);
                }
            }
            //X or up key pressed.
            else if(k == ALLEGRO_KEY_UP || k == ALLEGRO_KEY_X)
            {
                //Try to rotate the pentomino clockwise.
                if(this->getBoard()->rotateRight())
                {
                    al_play_sample(this->getSFX(SFX_ROTATE),UTIL_SFX_VOLUME-0.3,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);
                }
                else
                {
                    al_play_sample(this->getSFX(SFX_NOMOVE),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);
                }
            }
            //C key pressed.
            if(k == ALLEGRO_KEY_C)
            {
                //Hold if possible.
                if(this->getHolds())
                {
                    this->hold();
                    al_play_sample(this->getSFX(SFX_HOLD),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);
                }
            }
        }
        if(e.type == ALLEGRO_EVENT_KEY_UP) {
            int k = e.keyboard.keycode;
            this->setKey(k,false);
            if(k == ALLEGRO_KEY_DOWN)
            {
                //Down key released. Reset soft drop timer.
                al_set_timer_speed(this->getSoftDropTimer(),this->getBoard()->getSpeed());
            }
            if(!this->getKey(ALLEGRO_KEY_LEFT) && !this->getKey(ALLEGRO_KEY_RIGHT))
            {
                //Left and right keys both released. Reset DAS timer.
                al_set_timer_speed(this->getAutoShiftTimer(),UTIL_AUTO_SHIFT_DELAY);
                al_stop_timer(this->getAutoShiftTimer());
            }
        }
    }

    //Draw only if anything actually changed.
    if(this->getDrawSwitch())
    {
        //Target the screen.
        al_set_target_backbuffer(this->getDisplay());

        //Display solid color background.
        al_clear_to_color(al_map_rgb(0,0,0));

        //Draw static text.
        //TODO: Draw this part exactly once.

        //Draw title.
        al_draw_text(this->getFont(),
                     al_map_rgb(255,255,255),
                     (UTIL_PREVIEW_X + UTIL_PREVIEW_WIDTH + 2*UTIL_FRAME_THICKNESS + UTIL_SCREEN_WIDTH)/2,
                     UTIL_BLOCK_SIZE,
                     ALLEGRO_ALIGN_CENTER,
                     "PENTRIS");

        //Draw instructions.
        al_draw_text(this->getFont(),
                     al_map_rgb(255,255,255),
                     (UTIL_PREVIEW_X + UTIL_PREVIEW_WIDTH + 2*UTIL_FRAME_THICKNESS + UTIL_SCREEN_WIDTH)/2,
                     5*UTIL_BLOCK_SIZE,
                     ALLEGRO_ALIGN_CENTER,
                     "Controls");

        al_draw_text(this->getFont(),
                     al_map_rgb(255,255,255),
                     UTIL_PREVIEW_X + UTIL_PREVIEW_WIDTH + 2*UTIL_BLOCK_SIZE,
                     7*UTIL_BLOCK_SIZE,
                     ALLEGRO_ALIGN_LEFT,
                     "+: Move");

        al_draw_text(this->getFont(),
                     al_map_rgb(255,255,255),
                     UTIL_PREVIEW_X + UTIL_PREVIEW_WIDTH + 2*UTIL_BLOCK_SIZE,
                     8*UTIL_BLOCK_SIZE,
                     ALLEGRO_ALIGN_LEFT,
                     "Space: Drop");

        al_draw_text(this->getFont(),
                     al_map_rgb(255,255,255),
                     UTIL_PREVIEW_X + UTIL_PREVIEW_WIDTH + 2*UTIL_BLOCK_SIZE,
                     9*UTIL_BLOCK_SIZE,
                     ALLEGRO_ALIGN_LEFT,
                     "Z: Rotate Left");

        al_draw_text(this->getFont(),
                     al_map_rgb(255,255,255),
                     UTIL_PREVIEW_X + UTIL_PREVIEW_WIDTH + 2*UTIL_BLOCK_SIZE,
                     10*UTIL_BLOCK_SIZE,
                     ALLEGRO_ALIGN_LEFT,
                     "X: Rotate Right");

        al_draw_text(this->getFont(),
                     al_map_rgb(255,255,255),
                     UTIL_PREVIEW_X + UTIL_PREVIEW_WIDTH + 2*UTIL_BLOCK_SIZE,
                     11*UTIL_BLOCK_SIZE,
                     ALLEGRO_ALIGN_LEFT,
                     "C: Hold");

        al_draw_text(this->getFont(),
                     al_map_rgb(255,255,255),
                     UTIL_PREVIEW_X + UTIL_PREVIEW_WIDTH + 2*UTIL_BLOCK_SIZE,
                     12*UTIL_BLOCK_SIZE,
                     ALLEGRO_ALIGN_LEFT,
                     "N: New Game");

        al_draw_text(this->getFont(),
                     al_map_rgb(255,255,255),
                     UTIL_PREVIEW_X + UTIL_PREVIEW_WIDTH + 2*UTIL_BLOCK_SIZE,
                     13*UTIL_BLOCK_SIZE,
                     ALLEGRO_ALIGN_LEFT,
                     "P: Pause");

        al_draw_textf(this->getFont(),
                     al_map_rgb(255,255,255),
                     UTIL_PREVIEW_X + UTIL_PREVIEW_WIDTH + 2*UTIL_BLOCK_SIZE,
                     14*UTIL_BLOCK_SIZE,
                     ALLEGRO_ALIGN_LEFT,
                     "M: Music (Type%c)",65+this->getMusic());

        al_draw_textf(this->getFont(),
                     al_map_rgb(255,255,255),
                     UTIL_PREVIEW_X + UTIL_PREVIEW_WIDTH + 2*UTIL_BLOCK_SIZE,
                     20*UTIL_BLOCK_SIZE,
                     ALLEGRO_ALIGN_LEFT,
                     "Level: %i",
                     this->getBoard()->getLevel());

        al_draw_textf(this->getFont(),
                     al_map_rgb(255,255,255),
                     UTIL_PREVIEW_X + UTIL_PREVIEW_WIDTH + 2*UTIL_BLOCK_SIZE,
                     21*UTIL_BLOCK_SIZE,
                     ALLEGRO_ALIGN_LEFT,
                     "Lines Needed: %-i",
                     this->getBoard()->getLinesRemaining());

        al_draw_textf(this->getFont(),
                     al_map_rgb(255,255,255),
                     UTIL_PREVIEW_X + UTIL_PREVIEW_WIDTH + 2*UTIL_BLOCK_SIZE,
                     23*UTIL_BLOCK_SIZE,
                     ALLEGRO_ALIGN_LEFT,
                     "Score: %li",
                     this->getBoard()->getScore());


        //Frame the preview.
        ALLEGRO_BITMAP* previewFrame =
            al_create_sub_bitmap(al_get_backbuffer(this->getDisplay()),
                                 UTIL_PREVIEW_X,
                                 UTIL_PREVIEW_Y - UTIL_FRAME_THICKNESS,
                                 UTIL_PREVIEW_WIDTH + 2*UTIL_FRAME_THICKNESS,
                                 UTIL_PREVIEW_HEIGHT + 2*UTIL_FRAME_THICKNESS);

        al_set_target_bitmap(previewFrame);
        al_clear_to_color(al_map_rgb(255,255,255));

        //Draw the preview and any blocks contained within.
        ALLEGRO_BITMAP* preview =
            al_create_sub_bitmap(previewFrame,
                                 UTIL_FRAME_THICKNESS,
                                 UTIL_FRAME_THICKNESS,
                                 UTIL_PREVIEW_WIDTH,
                                 UTIL_PREVIEW_HEIGHT);
        al_set_target_bitmap(preview);
        al_clear_to_color(al_map_rgb(0,0,0));

        //...Unless game is paused.
        if(!this->getPaused())
        {
            for(int i=0; i<5; i++)
            {
                for(int j=0; j < 4*UTIL_PREVIEW_NUMBER; j++)
                {
                    if(this->getPreview()->getCell(i,j))
                    {
                        al_draw_tinted_bitmap(this->getPreview()->getCell(i,j)->getSprite(),
                                              this->getPreview()->getCell(i,j)->getTint(),
                                              UTIL_BLOCK_SIZE*i,
                                              UTIL_BLOCK_SIZE*j,
                                              0);
                    }
                }
            }
        }

        //Destroy all sub bitmaps.
        al_destroy_bitmap(preview);
        preview = nullptr;
        al_destroy_bitmap(previewFrame);
        previewFrame = nullptr;

        //Frame the hold.
        ALLEGRO_BITMAP* holdFrame =
            al_create_sub_bitmap(al_get_backbuffer(this->getDisplay()),
                                 UTIL_HOLD_X - 2*UTIL_FRAME_THICKNESS,
                                 UTIL_HOLD_Y - UTIL_FRAME_THICKNESS,
                                 UTIL_HOLD_WIDTH + 2*UTIL_FRAME_THICKNESS,
                                 UTIL_HOLD_HEIGHT + 2*UTIL_FRAME_THICKNESS);

        al_set_target_bitmap(holdFrame);
        al_clear_to_color(al_map_rgb(255,255,255));

        //Draw the hold and any blocks contained within.
        ALLEGRO_BITMAP* hold =
            al_create_sub_bitmap(holdFrame,
                             UTIL_FRAME_THICKNESS,
                             UTIL_FRAME_THICKNESS,
                             UTIL_HOLD_WIDTH,
                             UTIL_HOLD_HEIGHT);
        al_set_target_bitmap(hold);
        al_clear_to_color(al_map_rgb(0,0,0));

        //...Unless game is paused.
        if(!this->getPaused())
        {
            for(int i=0; i<5; i++)
            {
                for(int j=0; j < 4*UTIL_HOLD_NUMBER; j++)
                {
                    if(this->getHold()->getCell(i,j))
                    {
                        al_draw_tinted_bitmap(this->getHold()->getCell(i,j)->getSprite(),
                                              this->getHold()->getCell(i,j)->getTint(),
                                              UTIL_BLOCK_SIZE*i,
                                              UTIL_BLOCK_SIZE*j,
                                              0);
                    }
                }
            }
        }

        //Destroy all sub bitmaps.
        al_destroy_bitmap(hold);
        hold = nullptr;
        al_destroy_bitmap(holdFrame);
        holdFrame = nullptr;

        //Frame the board.
        ALLEGRO_BITMAP* boardFrame =
            al_create_sub_bitmap(al_get_backbuffer(this->getDisplay()),
                                 UTIL_BOARD_X - UTIL_FRAME_THICKNESS,
                                 UTIL_BOARD_Y - UTIL_FRAME_THICKNESS,
                                 UTIL_BOARD_WIDTH + 2*UTIL_FRAME_THICKNESS,
                                 UTIL_BOARD_HEIGHT + 2*UTIL_FRAME_THICKNESS);

        al_set_target_bitmap(boardFrame);
        al_clear_to_color(al_map_rgb(255,255,255));

        //Draw the board and any blocks contained within.
        ALLEGRO_BITMAP* board =
            al_create_sub_bitmap(boardFrame,
                                 UTIL_FRAME_THICKNESS,
                                 UTIL_FRAME_THICKNESS,
                                 UTIL_BOARD_WIDTH,
                                 UTIL_BOARD_HEIGHT);
        al_set_target_bitmap(board);
        al_clear_to_color(al_map_rgb(0,0,0));

        //...Unless game is paused.
        if(!this->getPaused())
        {
            for(int i=0; i<UTIL_GRID_WIDTH; i++)
            {
                for(int j=-2; j<UTIL_GRID_HEIGHT; j++)
                {
                    if(this->getBoard()->getCell(i,j))
                    {
                        if(this->getBoard()->getCell(i,j)->getLiveSwitch())
                        {
                            //Block is live. Draw the block's shadow.
                            al_draw_tinted_bitmap(this->getBoard()->getCell(i,j)->getSprite(),
                                                  al_premul_rgba(100,100,100,100),
                                                  UTIL_BLOCK_SIZE*i,
                                                  UTIL_BLOCK_SIZE*(j+this->getBoard()->getCurrentPentomino()->getShadow()),
                                                  0);
                        }

                        al_draw_tinted_bitmap(this->getBoard()->getCell(i,j)->getSprite(),
                                              this->getBoard()->getCell(i,j)->getTint(),
                                              UTIL_BLOCK_SIZE*i,
                                              UTIL_BLOCK_SIZE*j,
                                              0);
                    }
                }
            }


        }
        else if(this->getPaused() != UTIL_UNPAUSE_TIME+1)
        {
            //Unpause in progress. Draw unpause timer ticks.
            al_draw_textf(this->getFont(),
                     al_map_rgb(255,255,255),
                     UTIL_BOARD_WIDTH/2,
                     UTIL_BOARD_HEIGHT/2,
                     ALLEGRO_ALIGN_CENTER,
                     "%i...",
                     this->getPaused());
        }
        else
        {
            //Indicate pause.
            al_draw_text(this->getFont(),
                     al_map_rgb(255,255,255),
                     UTIL_BOARD_WIDTH/2,
                     UTIL_BOARD_HEIGHT/2,
                     ALLEGRO_ALIGN_CENTER,
                     "Pause");
        }

        //Destroy all sub bitmaps.
        al_destroy_bitmap(board);
        board = nullptr;
        al_destroy_bitmap(boardFrame);
        boardFrame = nullptr;

        //Do it!
        al_flip_display();
        this->setDrawSwitch(false);
    }

    return;
}


//Set up a new game.
void MainLoop::newGame()
{
    //Kill current game, if any.
    al_stop_timer(this->getGameOverTimer());
    if(this->getBoard()!= nullptr)
    {
        delete this->getBoard();
    }
    if(this->getPreview() != nullptr)
    {
        delete this->getPreview();
    }
    if(this->getHold() != nullptr)
    {
        delete this->getHold();
    }

    //Start new game.
    al_stop_samples();
    this->setGameOver(false);
    this->setPaused(UTIL_UNPAUSE_TIME+1);
    this->getOrder().clear();
    this->setBagsize(0);
    for(int i = 0; i < UTIL_PREVIEW_NUMBER+1; i++)
    {
        this->drawPentomino();
    }

    this->setBoard(new Board());
    this->getBoard()->spawnPentomino(this->getOrder()[0]);
    this->getOrder().erase(this->getOrder().begin());
    this->setPreview(new Preview(this->getOrder()));
    this->setHold(new Hold());

    al_set_timer_speed(this->getSoftDropTimer(),1.0);
    al_set_timer_speed(this->getAutoShiftTimer(),UTIL_AUTO_SHIFT_DELAY);

    al_start_timer(this->getUnpauseTimer());
    al_start_timer(this->getSoftDropTimer());

    this->unpause();

    this->setDrawSwitch(true);
}

//Die :/
void MainLoop::die()
{
    al_start_timer(this->getGameOverTimer());
    al_stop_samples();
    return;
}

//Unpause.
void MainLoop::unpause()
{
    this->setPaused(this->getPaused()-1);

    //Resume the game.
    if(!this->getPaused())
    {
        al_stop_timer(this->getUnpauseTimer());
        al_play_sample(this->getBGM(),UTIL_BGM_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_LOOP,nullptr);
    }
    else if(this->getPaused() == 1)
    {
        al_play_sample(this->getSFX(SFX_UNPAUSE),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
    }
    else
    {
        al_play_sample(this->getSFX(SFX_START_TIMER),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
    }
}

//Set the order of pentominoes to come.
void MainLoop::drawPentomino()
{
    //Fill the bag if it's empty.
    if(!this->getBagsize())
    {
        //Load the bag.
        unsigned char bag[7];
        //Draw K, N, or W.
        unsigned char red = uid(mt_generator)%3;

        switch(red)
        {
            case 0:         bag[0] = PENTOMINO_K; break;
            case 1:         bag[0] = PENTOMINO_N; break;
            case 2:default: bag[0] = PENTOMINO_W; break;
        }

        //Draw C, L, or Z.
        unsigned char orange = uid(mt_generator)%3;

        switch(orange)
        {
            case 0:         bag[1] = PENTOMINO_C; break;
            case 1:         bag[1] = PENTOMINO_L; break;
            case 2:default: bag[1] = PENTOMINO_Z; break;
        }

        //Draw P or Q.
        unsigned char yellow = uid(mt_generator)%2;

        switch(yellow)
        {
            case 0:         bag[2] = PENTOMINO_P; break;
            case 1:default: bag[2] = PENTOMINO_Q; break;
        }

        //Draw F or U.
        unsigned char green = uid(mt_generator)%2;

        switch(green)
        {
            case 0:         bag[3] = PENTOMINO_F; break;
            case 1:default: bag[3] = PENTOMINO_U; break;
        }

        //Draw J, S, or V.
        unsigned char blue = uid(mt_generator)%3;

        switch(blue)
        {
            case 0:         bag[4] = PENTOMINO_J; break;
            case 1:         bag[4] = PENTOMINO_S; break;
            case 2:default: bag[4] = PENTOMINO_V; break;
        }

        //Draw D, T, X, or Y.
        unsigned char purple = uid(mt_generator)%4;

        switch(purple)
        {
            case 0:         bag[5] = PENTOMINO_D; break;
            case 1:         bag[5] = PENTOMINO_T; break;
            case 2:         bag[5] = PENTOMINO_X; break;
            case 3:default: bag[5] = PENTOMINO_Y; break;
        }

        //Last piece is always I.
        bag[6] = PENTOMINO_I;

        //Shuffle the bag.
        std::random_shuffle(std::begin(bag),std::end(bag));

        //Store the bag.
        for(int i = 0; i < 7; i++)
        {
            this->setBag(i,bag[i]);
        }
        this->setBagsize(7);
    }

    //Draw the next pentomino out of the bag.
    this->getOrder().push_back(this->getBag(this->getBagsize()-1));
    this->setBagsize(this->getBagsize()-1);
}

//Try to soft drop.
void MainLoop::trySoftDrop()
{
    //Check if this pentomino is touching something.
    if(al_get_timer_started(this->getLockDelayTimer()))
    {
        //Release lock delay and restart soft drop timer.
        al_stop_timer(this->getLockDelayTimer());
        al_start_timer(this->getSoftDropTimer());

        //Try once more to soft drop.
        if(this->getBoard()->softDrop())
        {
            //Couldn't soft drop after lock delay. Lock this pentomino, try to score and spawn a new one.
            al_play_sample(this->getSFX(SFX_LOCK),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);
            delete this->getBoard()->getCurrentPentomino();
            int l = this->getBoard()->clearLines();
            switch(l)
            {
                case 1:     al_play_sample(this->getSFX(SFX_CLEAR_SINGLE),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);       break;
                case 2:     al_play_sample(this->getSFX(SFX_CLEAR_DOUBLE),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);       break;
                case 3:     al_play_sample(this->getSFX(SFX_CLEAR_TRIPLE),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);       break;
                case 4:     al_play_sample(this->getSFX(SFX_CLEAR_QUADRUPLE),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);    break;
                case 5:     al_play_sample(this->getSFX(SFX_CLEAR_PENTRIS),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);      break;
                default:    break;
            }

            this->trySpawn();

            //Also release holding.
            this->setHolds(UTIL_HOLDS_PER_TURN);
        }
    }

    //Only continue if game is still live.
    if(!this->getGameOver())
    {
        if(this->getBoard()->softDrop())
        {
            //Switch to lock delay.
            al_stop_timer(this->getSoftDropTimer());
            al_start_timer(this->getLockDelayTimer());
        }
    }
}

//Try to hard drop.
void MainLoop::tryHardDrop()
{
    //Check if new pentomino should be spawned.
    if(this->getBoard()->hardDrop())
    {
        //Lock this pentomino, try to score and spawn a new one.
        al_play_sample(this->getSFX(SFX_LOCK),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);
        delete this->getBoard()->getCurrentPentomino();
        int l = this->getBoard()->clearLines();
        switch(l)
        {
            case 1:     al_play_sample(this->getSFX(SFX_CLEAR_SINGLE),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);       break;
            case 2:     al_play_sample(this->getSFX(SFX_CLEAR_DOUBLE),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);       break;
            case 3:     al_play_sample(this->getSFX(SFX_CLEAR_TRIPLE),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);       break;
            case 4:     al_play_sample(this->getSFX(SFX_CLEAR_QUADRUPLE),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);    break;
            case 5:     al_play_sample(this->getSFX(SFX_CLEAR_PENTRIS),UTIL_SFX_VOLUME,0.0,1.0,ALLEGRO_PLAYMODE_ONCE,nullptr);      break;
            default:    break;
        }

        this->trySpawn();

        //Also release holding.
        this->setHolds(UTIL_HOLDS_PER_TURN);
    }
}

//Try to spawn a new pentomino.
void MainLoop::trySpawn()
{
    //Spawn new pentomino.
    this->setGameOver(!this->getBoard()->spawnPentomino(this->getOrder()[0])); //Die if pentomino cannot be spawned.
    al_set_timer_speed(this->getSoftDropTimer(),this->getBoard()->getSpeed());
    this->getOrder().erase(this->getOrder().begin());

    if(this->getGameOver())
    {
        //Die :/
        this->die();
    }
    this->drawPentomino();
    this->getPreview()->updatePreview(this->getOrder().back());
}

//Store the current pentomino in the hold, and spawn the pentomino from the hold if there is one.
void MainLoop::hold()
{
    //Erase this pentomino off the board.
    for(int i = 0; i < this->getBoard()->getCurrentPentomino()->getN(); i++)
    {
        char x = this->getBoard()->getCurrentPentomino()->getBlock(i)->getX();
        char y = this->getBoard()->getCurrentPentomino()->getBlock(i)->getY();
        delete this->getBoard()->getCell(x,y);
        this->getBoard()->setCell(x,y,nullptr);
    }

    unsigned char type = this->getHold()->updateHold(this->getBoard()->getCurrentPentomino()->getType());
    if(type != PENTOMINO_NULL)
    {
        //Spawn new pentomino.
        this->setGameOver(!this->getBoard()->spawnPentomino(type)); //Die if pentomino cannot be spawned.

        if(this->getGameOver())
        {
            //Die :/
            this->die();
        }
    }
    else
    {
        this->trySpawn();
    }

    //Restrict holding.
    this->setHolds(this->getHolds()-1);
}
