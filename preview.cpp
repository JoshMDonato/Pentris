#include "preview.h"

Preview::Preview(std::vector<unsigned char> v)
{
    //Setup the preview window. Each upcoming pentomino can fit in a 5x4 grid.
    for(int i=0; i<5; i++)
    {
        for(int j=0; j < 4*UTIL_PREVIEW_NUMBER; j++)
        {
            //Fill the board with empty cells.
            this->m_Preview.insert(std::make_pair(std::make_pair(i,j), nullptr));
        }
    }

    //Load the preview.
    for(unsigned int i = 0; i < UTIL_PREVIEW_NUMBER; i++)
    {
        this->updatePreview(v[i]);
    }
}

Preview::~Preview()
{
    //Check the entire preview for blocks.
    for(int i=0; i<5; i++)
    {
        for(int j=0; j < 4*UTIL_PREVIEW_NUMBER; j++)
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


void Preview::updatePreview(unsigned char uc)
{
    for(int i = 0; i < 5; i++)
    {
        //Destroy the top four rows.
        for(int j = 0; j < 4; j++)
        {
            if(this->getCell(i,j) != nullptr)
            {
                delete this->getCell(i,j);
            }
            this->setCell(i,j,nullptr);
        }

        //Move everything else up by four rows.
        for(int k = 4; k < 4*UTIL_PREVIEW_NUMBER; k++)
        {
            this->setCell(i,k-4,this->getCell(i,k));
            this->setCell(i,k,nullptr);
        }
    }

    //Update the last three rows with the newest pentomino.

    //Get the color of the blocks.
    ALLEGRO_COLOR clr = al_map_rgb(m_Tint[uc].uc_Red,m_Tint[uc].uc_Green,m_Tint[uc].uc_Blue);

    //Lookup initial positions of each block.
    std::pair<std::multimap<unsigned char, PENTOMINO_INITIAL_POSITION>::iterator, std::multimap<unsigned char, PENTOMINO_INITIAL_POSITION>::iterator>
        it = m_InitialPosition.equal_range(uc);

    //Actually place the blocks on the preview.
    for(std::multimap<unsigned char, PENTOMINO_INITIAL_POSITION>::iterator i = it.first; i != it.second; ++i)
    {
        this->setCell((uc != PENTOMINO_I) + i->second.x, 4*(UTIL_PREVIEW_NUMBER-1)+(uc == PENTOMINO_I) + i->second.y, new Block(clr));
    }
}
