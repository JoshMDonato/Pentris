#include "hold.h"

Hold::Hold()
{
    //Setup the hold window. The held pentomino can fit in a 5x4 grid.
    for(int i=0; i<5; i++)
    {
        for(int j=0; j< 5*UTIL_HOLD_NUMBER; j++)
        {
            //Fill the hold with empty cells.
            this->m_Hold.insert(std::make_pair(std::make_pair(i,j), nullptr));
        }
    }

    //Set all held pentominoes to null.
    for(int j = 0; j < UTIL_HOLD_NUMBER; j++)
    {
        this->setPentomino(j,PENTOMINO_NULL);
    }
}

Hold::~Hold()
{
    //Check the entire hold for blocks.
    for(int i=0; i<5; i++)
    {
        for(int j=0; j < 4*UTIL_HOLD_NUMBER; j++)
        {
            //If there is a block in this cell, delete it.
            if(this->getCell(i,j))
            {
                delete this->getCell(i,j);
                this->setCell(i,j,nullptr);
            }
        }
    }
}


//Updates the hold. Returns the pentomino to be unheld.
unsigned char Hold::updateHold(unsigned char uc)
{
    //Update the hold.
    for(int i = 0; i < 5; i++)
    {
        //Destroy the top five rows.
        for(int j = 0; j < 4; j++)
        {
            if(this->getCell(i,j) != nullptr)
            {
                delete this->getCell(i,j);
            }
            this->setCell(i,j,nullptr);
        }

        //Move everything else up by five rows.
        for(int k = 4; k < 4*UTIL_HOLD_NUMBER; k++)
        {
            this->setCell(i,k-4,this->getCell(i,k));
            this->setCell(i,k,nullptr);
        }
    }

    //Update the last four rows with the incoming pentomino.

    //Get the color of the blocks.
    ALLEGRO_COLOR clr = al_map_rgb(m_Tint[uc].uc_Red,m_Tint[uc].uc_Green,m_Tint[uc].uc_Blue);

    //Lookup initial positions of each block.
    std::pair<std::multimap<unsigned char, PENTOMINO_INITIAL_POSITION>::iterator, std::multimap<unsigned char, PENTOMINO_INITIAL_POSITION>::iterator>
        it = m_InitialPosition.equal_range(uc);

    //Actually place the blocks on the preview.
    for(std::multimap<unsigned char, PENTOMINO_INITIAL_POSITION>::iterator i = it.first; i != it.second; ++i)
    {
        this->setCell((uc != PENTOMINO_I) + i->second.x, 4*(UTIL_HOLD_NUMBER-1)+(uc == PENTOMINO_I) + i->second.y, new Block(clr));
    }


    //Record the actual pentomino.
    unsigned char r = this->getPentomino(0);
    this->setPentomino(UTIL_HOLD_NUMBER-1,uc);
    return r;
}
