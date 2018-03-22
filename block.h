
/*

    =================
    ===== BLOCK =====
    =================

    The main object. PENTOMINOES are made of 5 of these.
    Forming a horizontal line of BLOCKS will clear that line.

*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#include <utilities.h>

#ifndef BLOCK_H
#define BLOCK_H

class Block
{
    public:
    Block(ALLEGRO_COLOR clr); //Constructor
    ~Block();                 //Destructor

    void            drop()          {this->i_y++;}  //Same as setY(getY()+1).


    //Gets
    ALLEGRO_BITMAP* getSprite()     {return this->bmp_Sprite;}
    ALLEGRO_COLOR   getTint()       {return this->clr_Tint;}
    int             getX()          {return this->i_x;}
    int             getY()          {return this->i_y;}
    char            getDx()         {return this->c_dx;}
    char            getDy()         {return this->c_dy;}
    bool            getLiveSwitch() {return this->b_isLive;}

    //Sets
    void setSprite(ALLEGRO_BITMAP* bmp) {this->bmp_Sprite = bmp;}
    void setTint(ALLEGRO_COLOR clr)     {this->clr_Tint = clr;}
    void setX(int i)                    {this->i_x = i;}
    void setY(int i)                    {this->i_y = i;}
    void setDx(char c)                  {this->c_dx = c;}
    void setDy(char c)                  {this->c_dy = c;}
    void setLiveSwitch(bool b)          {this->b_isLive = b;}


    private:
    ALLEGRO_BITMAP*     bmp_Sprite  = nullptr;      //Sprite
    ALLEGRO_COLOR       clr_Tint    ;               //Tint
    int                 i_x, i_y    ;               //Position
    char                c_dx, c_dy  ;               //Rotation
    bool                b_isLive    = true;

};

#endif //BLOCK_H
