
/*

    ===================
    ===== PREVIEW =====
    ===================

    Upcoming pentominoes are shown here.
    Pentomino spawn order is predetermined in the main loop.
    This window is only to assist the player.

*/

#include <map>
#include <iterator>

#include <utilities.h>
#include <pentomino.h>
#include <block.h>

#ifndef PREVIEW_H
#define PREVIEW_H


class Preview
{
    public:
    Preview(std::vector<unsigned char> v);  //Constructor
    ~Preview();                             //Destructor

    void updatePreview(unsigned char uc);   //Updates the preview.


    //Gets
    Block*  getCell(int x, int y)                       {return this->m_Preview.find(std::make_pair(x,y))->second;}

    //Sets
    void    setCell(int x, int y, Block* b)             {this->m_Preview.find(std::make_pair(x,y))->second = b;}


    private:
    std::map<std::pair<int, int>, Block*>   m_Preview   ; //The entire preview and all of its blocks.
};

#endif //PREVIEW_H
