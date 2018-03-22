#include "pentomino.h"

Pentomino::Pentomino(unsigned char uc_type)
{
    //Randomly decide the configuration.
    //TODO: Optimize randomization and fairness.
    srand(time(nullptr));
    this->setType(uc_type);

    ALLEGRO_COLOR clr = al_map_rgb( m_Tint[this->getType()].uc_Red,
                                    m_Tint[this->getType()].uc_Green,
                                    m_Tint[this->getType()].uc_Blue);

    //Add the blocks to the pentomino.
    for(int i = 0; i < this->uc_N; i++)
    {
        this->b_Blocks.insert(this->b_Blocks.begin()+i, new Block(clr));
    }

    //Lookup initial positions of each block.
    std::pair<std::multimap<unsigned char, PENTOMINO_INITIAL_POSITION>::iterator, std::multimap<unsigned char, PENTOMINO_INITIAL_POSITION>::iterator>
        it = m_InitialPosition.equal_range(this->getType());

    //Actually place the blocks on the board.
    for(std::multimap<unsigned char, PENTOMINO_INITIAL_POSITION>::iterator i = it.first; i != it.second; ++i)
    {
        this->b_Blocks.at(i->second.n)->setX(UTIL_INITIAL_X + i->second.x);
        this->b_Blocks.at(i->second.n)->setY(UTIL_INITIAL_Y + i->second.y);

        //Also record the rotation data.
        this->b_Blocks.at(i->second.n)->setDx(i->second.dx);
        this->b_Blocks.at(i->second.n)->setDy(i->second.dy);
    }
}

Pentomino::~Pentomino()
{
    //Flag each of these pentomino's blocks to dead blocks.
    for (int i = 0; i < this->uc_N; i++)
    {
        this->getBlock(i)->setLiveSwitch(false);
    }
}


//Pull data off the kickoff table.
std::pair<int,int> Pentomino::pullKickoffData(int i, int ri, int rf)
{
    PENTOMINO_KICKOFF it = m_Kickoffs[std::make_pair(this->getType(),std::make_pair(ri,rf))];
    switch(i)
    {
        case 1:     return std::make_pair(it.kx1,it.ky1);
        case 2:     return std::make_pair(it.kx2,it.ky2);
        case 3:     return std::make_pair(it.kx3,it.ky3);
        case 4:     return std::make_pair(it.kx4,it.ky4);
        default:    break;
    }
    return std::make_pair(0,0);
}
