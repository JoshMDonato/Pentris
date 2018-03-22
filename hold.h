
/*

    ================
    ===== HOLD =====
    ================

    Held pentominoes are shown here.
    During gameplay, the current pentomino can be swapped with one stored here.
    Initially the hold is empty, and current pentominoes can be stored until full.
    Normally there is only one held pentomino.

*/

#include <map>
#include <iterator>

#include <utilities.h>
#include <pentomino.h>
#include <block.h>

#ifndef HOLD_H
#define HOLD_H


class Hold
{
    public:
    Hold();     //Constructor
    ~Hold();    //Destructor

    unsigned char updateHold(unsigned char uc);             //Updates the hold. Returns the pentomino to be unheld.


    //Gets
    Block*          getCell(int x, int y)               {return this->m_Hold.find(std::make_pair(x,y))->second;}
    unsigned char   getPentomino(unsigned char uc)      {return this->uc_HeldPentomino[uc];}

    //Sets
    void setCell(int x, int y, Block* b)                {this->m_Hold.find(std::make_pair(x,y))->second = b;}
    void setPentomino(unsigned char uc, unsigned char p){this->uc_HeldPentomino[uc] = p;}


    private:
    std::map<std::pair<int, int>, Block*>   m_Hold      ;    //The entire hold window and all of its blocks.
    unsigned char uc_HeldPentomino[UTIL_HOLD_NUMBER]    ;    //The held pentominoes.
};

#endif //HOLD_H
