
/*

    =================
    ===== BOARD =====
    =================

    All of the action takes place in this board.
    The board is comprised of a grid of blocks.
    Forming a full horizontal line of blocks will clear that line.

*/

#include <map>
#include <iterator>

#include <utilities.h>
#include <pentomino.h>

#ifndef BOARD_H
#define BOARD_H


class Board
{
    public:
    Board();                //Constructor
    ~Board();               //Destructor

    bool spawnPentomino(unsigned char uc_type); //Spawns a new pentomino on the top of the board. True if a new pentomino can be spawned.
    bool softDrop();                            //Moves the current pentomino if possible, or grounds it. True if pentomino is touching something.
    bool hardDrop();                            //Moves the current pentomino as far down as possible. True if a new pentomino should be spawned.
    int  clearLines();                          //Tries to clear full lines of dead blocks. Returns the number of lines cleared.
    bool spinBonus();                           //Checks for spin bonuses. True if there will be a bonus.
    bool moveLeft();                            //Left key pressed. Attempt to move left. True if move was successful.
    bool moveRight();                           //Right key pressed. Attempt to move right. True if move was successful.
    bool rotateRight();                         //Rotate clockwise. True if rotation was successful.
    bool rotateLeft();                          //Rotate counterclockwise. True if rotation was successful.
    void findShadow();                          //Finds the shadow of the current pentomino.
    bool killBoard();                           //Game over. Fill the board with black blocks. True if animation is still in progress.


    //Inlines
    double getSpeed()                {return std::pow(UTIL_GRAVITY_ALPHA - UTIL_GRAVITY_BETA*(this->getLevel()-1),this->getLevel()-1);}
    void addScore(unsigned int ui)   {this->setScore(this->getScore()+ui);}

    //Gets
    Block*          getCell(int x, int y)   {return this->m_Board.find(std::make_pair(x,y))->second;}
    Pentomino*      getCurrentPentomino()   {return this->p_CurrentPentomino;}
    unsigned char   getLevel()              {return this->uc_Level;}
    unsigned int    getLines()              {return this->ui_Lines;}
    char            getLinesRemaining()     {return this->c_LinesRemaining;}
    unsigned long   getScore()              {return this->l_Score;}
    unsigned int    getCombo()              {return this->ui_Combo;}
    unsigned char   getLastScoreType()      {return this->uc_LastScoreType;}

    //Sets
    void setCell(int x, int y, Block* b)
    {
        if(b != nullptr)
        {
            b->setX(x);
            b->setY(y);
        }
        this->m_Board.find(std::make_pair(x,y))->second = b;
    }
    void setCurrentPentomino(Pentomino* p)  {this->p_CurrentPentomino = p;}
    void setLevel(unsigned char uc)         {this->uc_Level = uc;}
    void setLines(unsigned int ui)          {this->ui_Lines = ui;}
    void setLinesRemaining(char c)          {this->c_LinesRemaining = c;}
    void setScore(unsigned long l)          {this->l_Score = l;}
    void setCombo(unsigned int ui)          {this->ui_Combo = ui;}
    void setLastScoreType(unsigned char uc) {this->uc_LastScoreType = uc;}


    //Variables
    private:
    std::map<std::pair<int, int>, Block*>   m_Board;    //The entire board and all of its blocks.
    Pentomino*      p_CurrentPentomino      = nullptr;  //The current pentomino.
    unsigned char   uc_Level                = 1;        //Current level.
    unsigned int    ui_Lines                = 0;        //Total lines cleared this game.
    char            c_LinesRemaining        = 5;        //Lines remaining to level up.
    unsigned long   l_Score                 = 0;        //Current score.
    unsigned int    ui_Combo                = 0;        //Current combo.
    unsigned char   uc_LastScoreType        = 0;        //Tracks the last type of score.


};

#endif //BOARD_H
