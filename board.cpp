#include <board.h>

using namespace std;

Board::Board()
{
    //Setup the board.
    for(int i=0; i<UTIL_GRID_WIDTH; i++)
    {
        for(int j=-2; j<UTIL_GRID_HEIGHT; j++)
        {
            //Fill the board with empty cells.
            this->m_Board.insert(std::make_pair(std::make_pair(i,j), nullptr));
        }
    }
}

Board::~Board()
{
    //Check the entire board for blocks.
    for(int i=0; i<UTIL_GRID_WIDTH; i++)
    {
        for(int j=-2; j<UTIL_GRID_HEIGHT; j++)
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


//Spawns a new pentomino on the top of the board.
bool Board::spawnPentomino(unsigned char uc_type)
{
    //Actually spawn the pentomino.
    this->setCurrentPentomino(new Pentomino(uc_type));

    //Put the blocks on the board.
    for(int i = 0; i < this->getCurrentPentomino()->getN(); i++)
    {
        int x = this->getCurrentPentomino()->getBlock(i)->getX();
        int y = this->getCurrentPentomino()->getBlock(i)->getY();

        if(this->getCell(x,y))
        {
            //New pentomino cannot be spawned over dead blocks. Abort spawn.
            return false;
        }
        else
        {
            this->setCell(x,y,this->getCurrentPentomino()->getBlock(i));
        }
    }

    //Find the shadow.
    this->findShadow();

    return true;
}

//Moves the current pentomino if possible, or grounds it. True if a new pentomino should be spawned.
bool Board::softDrop()
{
    bool canMove = true;

    //Check if it's possible to move it.
    for(int i = 0; i < this->getCurrentPentomino()->getN(); i++)
    {
        char x = this->getCurrentPentomino()->getBlock(i)->getX();
        char y = this->getCurrentPentomino()->getBlock(i)->getY();

        bool bottom = (y+1>=UTIL_GRID_HEIGHT);             //This block -is- on the bottom row.
        bool contact = false;                               //Is there a block directly under this one?

        if(this->getCell(x,y+1) != nullptr)
        {
            //There -is- a block directly under this one. Is it a dead block?
            contact = true;
            if(this->getCell(x,y+1)->getLiveSwitch())
            {
                contact = false; //The block under of this one still live and will also move down; disregard.
            }
            else
            {
                canMove = false; //Dead block detected. Abort drop.
                break;
            }
        }

        if(bottom || contact)
        {
            //This pentomino -cannot- be moved downward.
            canMove = false;
            break;
        }
    }

    if(canMove)
    {
        //Actually move the entire pentomino.

        //Cut the pentomino out of the board and store it temporarily.
        Block* blocks[5];

        for(int i = 0; i < this->getCurrentPentomino()->getN(); i++)
        {
            blocks[i] = this->getCurrentPentomino()->getBlock(i);       //Store this block.
            this->setCell(blocks[i]->getX(),blocks[i]->getY(),nullptr); //Erase this block off the board.
            blocks[i]->drop();                                          //Move this block down.
        }

        //Paste the pentomino one line down.
        for(int j = 0; j < this->getCurrentPentomino()->getN(); j++)
        {
            this->setCell(blocks[j]->getX(),blocks[j]->getY(),blocks[j]);
        }

        //Adjust the shadow.
        this->getCurrentPentomino()->setShadow(this->getCurrentPentomino()->getShadow()-1);

        //Score 1 point per level.
        this->addScore(this->getLevel());
        return false;
    }
    else
    {
        //Either set lock delay or spawn a new pentomino.
        return true;
    }

}

//Moves the current pentomino as far down as possible. True if a new pentomino should be spawned.
bool Board::hardDrop()
{
    //Teleport the entire pentomino down to its shadow.
    Pentomino* p = this->getCurrentPentomino();

    //Cut the pentomino out of the board and store it temporarily.
    Block* b[5];

    for(int i = 0; i < p->getN(); i++)
    {
        b[i] = p->getBlock(i);
        this->setCell(b[i]->getX(),b[i]->getY(),nullptr);
    }

    //Paste the pentomino in place.
    for(int j = 0; j < p->getN(); j++)
    {
        this->setCell(b[j]->getX(),b[j]->getY()+p->getShadow(),b[j]);
    }

    //Score 2 points per level per row dropped.
    this->addScore(2*this->getLevel()*p->getShadow());

    //Hard drops lock and kill the pentomino instantly.
    return true;
}

//Tries to clear full lines of dead blocks. Returns the number of lines cleared.
int Board::clearLines()
{
    //First, check special bonuses from D, T, and Y pentominoes.
    bool s = this->spinBonus();

    int l = 0;
    for(int j = 0; j < UTIL_GRID_HEIGHT; j++)
    {
        //Check top to bottom.
        int n = 0;
        for(int i = 0; i < UTIL_GRID_WIDTH; i++)
        {
            if(this->getCell(i,j) != nullptr)
            {
                n++;
            }
            else
            {
                //Line has an empty space; skip to next line.
                break;
            }
        }

        if(n == UTIL_GRID_WIDTH)
        {
            //Full line formed. Erase it and score!
            for(int i = 0; i < UTIL_GRID_WIDTH; i++)
            {
                delete this->getCell(i,j);
                this->setCell(i,j,nullptr);
            }
            //Drop everything above this line, bottom to top.
            for(int a = j-1; a >= 0; a--)
            {
                for(int i = 0; i < UTIL_GRID_WIDTH; i++)
                {
                    this->setCell(i,a+1,this->getCell(i,a));
                    this->setCell(i,a,nullptr);
                }
            }
            l++;
        }
    }

    //Try to level up.
    this->setLinesRemaining(this->getLinesRemaining()-l);
    if(this->getLinesRemaining() <= 0)
    {
        this->setLevel(this->getLevel()+1);
        this->setLinesRemaining(5*this->getLevel() + this->getLinesRemaining());
    }

    //Record the type of scoring and compare it to the last type.
    unsigned char b = this->getLastScoreType();
    this->setLastScoreType(10*(1 + s*l + !s*(l == 5)));
    //If the same type of scoring occurred back to back, add a bonus to the score.
    b = b*(b == this->getLastScoreType());

    //Tally combo count. Add a bonus to the score based on total combo.
    if(l>0)
    {
        this->setCombo(this->getCombo()+1);
    }
    else
    {
        this->setCombo(0);
    }

    //Score based on lines cleared, spin bonus, and any combos.
    this->addScore(this->getLevel()*100*(b + this->getCombo() + 4*s*(l+1) + !s*(l*(l+1)/2)));

    return l;
}

//Checks for spin bonuses. Returns the points gained.
bool Board::spinBonus()
{
    //Abort check if this is the first pentomino of the game.
    if(!this->getCurrentPentomino())
    {
        return false;
    }

    //Check pentomino type.
    unsigned char t = this->getCurrentPentomino()->getType();
    //Record the "center" block based on the type.
    unsigned char b = 0;

    switch(t)
    {
        case PENTOMINO_D: b = 2; break;
        case PENTOMINO_T: b = 3; break;
        case PENTOMINO_Y: b = 3; break;
        default: return 0;
    }

    unsigned char x = this->getCurrentPentomino()->getBlock(b)->getX();
    unsigned char y = this->getCurrentPentomino()->getBlock(b)->getY();

    unsigned char spin =
        (this->getCell(x-1,y-1) != nullptr) +
        (this->getCell(x+1,y-1) != nullptr) +
        (this->getCell(x-1,y+1) != nullptr) +
        (this->getCell(x+1,y+1) != nullptr);

    if(spin >= 3)
    {
        //Score spin bonus.
        return true;
    }

    return false;
}

//Left key pressed. Attempt to move left.
bool Board::moveLeft()
{
    bool canMove = true;

    //Check if it's possible to move each block.
    for(int i = 0; i < this->getCurrentPentomino()->getN(); i++)
    {
        int x = this->getCurrentPentomino()->getBlock(i)->getX();
        int y = this->getCurrentPentomino()->getBlock(i)->getY();

        bool left = (x<=0);             //This block -is- on the leftmost column.
        bool contact = false;           //Is there a block directly left of this one?

        if(this->getCell(x-1,y) != nullptr)
        {
            //There -is- a block directly left of this one. Is it a dead block?
            contact = true;
            if(this->getCell(x-1,y)->getLiveSwitch())
            {
                contact = false; //The block left of this one still live and will also move left; disregard.
            }
        }

        if(left || contact)
        {
            //This pentomino -cannot- be moved downward.
            return false;
        }
    }

    if(canMove)
    {
        //Actually move the entire pentomino.

        //Cut the pentomino out of the board and store it temporarily.
        Block* blocks[this->getCurrentPentomino()->getN()];

        for(int i = 0; i < this->getCurrentPentomino()->getN(); i++)
        {
            blocks[i] = this->getCurrentPentomino()->getBlock(i);       //Store this block.
            this->setCell(blocks[i]->getX(),blocks[i]->getY(),nullptr); //Erase this block off the board.
            if(blocks[i]->getX()>0){
            blocks[i]->setX(blocks[i]->getX()-1); }                     //Move this block left.
        }

        //Paste the pentomino one column left.
        for(int j = 0; j < this->getCurrentPentomino()->getN(); j++)
        {
            this->setCell(blocks[j]->getX(),blocks[j]->getY(),blocks[j]);
        }

        //Find the shadow.
        this->findShadow();
    }
    return true;
}

//Right key pressed. Attempt to move right.
bool Board::moveRight()
{
    bool canMove = true;

    //Check if it's possible to move it.
    for(int i = 0; i < this->getCurrentPentomino()->getN(); i++)
    {
        int x = this->getCurrentPentomino()->getBlock(i)->getX();
        int y = this->getCurrentPentomino()->getBlock(i)->getY();

        bool right = (x+1==UTIL_GRID_WIDTH);    //This block -is- on the rightmost column.
        bool contact = false;                   //Is there a block directly right of this one?

        if(this->getCell(x+1,y) != nullptr)
        {
            //There -is- a block directly right of this one. Is it a dead block?
            contact = true;
            if(this->getCell(x+1,y)->getLiveSwitch())
            {
                contact = false; //The block right of this one still live and will also move right; disregard.
            }
        }

        if(right || contact)
        {
            //This pentomino -cannot- be moved downward.
            return false;
        }
    }

    if(canMove)
    {
        //Actually move the entire pentomino.

        //Cut the pentomino out of the board and store it temporarily.
        Block* blocks[this->getCurrentPentomino()->getN()];

        for(int i = 0; i < this->getCurrentPentomino()->getN(); i++)
        {
            blocks[i] = this->getCurrentPentomino()->getBlock(i);       //Store this block.
            this->setCell(blocks[i]->getX(),blocks[i]->getY(),nullptr); //Erase this block off the board.
            blocks[i]->setX(blocks[i]->getX()+1);                       //Move this block right.
        }

        //Paste the pentomino one column left.
        for(int j = 0; j < this->getCurrentPentomino()->getN(); j++)
        {
            this->setCell(blocks[j]->getX(),blocks[j]->getY(),blocks[j]);
        }

        //Find the shadow.
        this->findShadow();
    }

    return true;
}

//Rotate clockwise.
bool Board::rotateRight()
{

    if(this->getCurrentPentomino()->getType() == PENTOMINO_X)
    {
        //X pentominoes have the same initial and final states; appear to rotate.
        return true;
    }

    bool canRotate = false;

    //Copy the current pentomino's blocks' positions and store them temporarily.
    int n = this->getCurrentPentomino()->getN();
    int x[n], y[n];
    int dx[n], dy[n];
    int px[n], py[n];
    char ri = this->getCurrentPentomino()->getOrientation()%4;
    char rf = (ri+1)%4;
    int kx = 0, ky = 0;

    //Store the initial positions.
    for(int i = 0; i < n; i++)
    {
        x[i] = this->getCurrentPentomino()->getBlock(i)->getX();
        y[i] = this->getCurrentPentomino()->getBlock(i)->getY();
        dx[i] = this->getCurrentPentomino()->getBlock(i)->getDy(); //Rotate clockwise.
        dy[i] = -this->getCurrentPentomino()->getBlock(i)->getDx();
        px[i] = x[i]+dx[i];
        py[i] = y[i]+dy[i];
    }


    for(int k = 0; k < 5; k++) //Try to rotate, and then try 4 different kickoffs plus rotation.
    {
        for(int i = 0; i < n; i++)
        {
            //Add in possible translations from kicking off walls.
            px[i] = x[i]+dx[i]+kx;
            py[i] = y[i]+dy[i]+ky;

            if(!px[i] && !py[i])
            {
                continue;   //This block rotates on itself and will not move; disregard.
            }

            //Block will possibly rotate out of bounds.
            if(px[i]<0 || px[i]>=UTIL_GRID_WIDTH || py[i]>=UTIL_GRID_HEIGHT)
            {
                canRotate = false;
                break;  //Block will rotate out of bounds; abort current rotation.
            }

            if(this->getCell(px[i],py[i]) != nullptr)
            {
                //There -is- a block directly where this block should go. Is it a dead block?
                canRotate = false;
                if(this->getCell(px[i],py[i])->getLiveSwitch())
                {
                    canRotate = true; //This block is still live and will also be moved; disregard.
                }
                else
                {
                    break;  //Dead block detected; abort current rotation.
                }
            }
        }
        if(canRotate)
        {
            break; //Rotation can proceed with current kick off.
        }
        else
        {
            //Try to kick off the dead blocks.
            std::pair<int,int> p = this->getCurrentPentomino()->pullKickoffData(k+1,ri,rf);
            kx = p.first;
            ky = p.second;
        }
    }

    if(canRotate)
    {
        for(int i = 0; i < n; i++)
        {
            //Erase this block off the board.
            this->setCell(x[i],y[i],nullptr);

            //Do it!
            this->getCurrentPentomino()->getBlock(i)->setX(px[i]);
            this->getCurrentPentomino()->getBlock(i)->setY(py[i]);
            this->getCurrentPentomino()->getBlock(i)->setDx(-dx[i]);
            this->getCurrentPentomino()->getBlock(i)->setDy(-dy[i]);
        }

        for(int j = 0; j < n; j++)
        {
            //Paste the rotated and translated block on the board.
            this->setCell(this->getCurrentPentomino()->getBlock(j)->getX(),
                          this->getCurrentPentomino()->getBlock(j)->getY(),
                          this->getCurrentPentomino()->getBlock(j));
            //Update orientation data.
            this->getCurrentPentomino()->setOrientation(rf);
        }

        //Find the shadow.
        this->findShadow();
    }

    return canRotate;

}

//Rotate counterclockwise.
bool Board::rotateLeft()
{

    if(this->getCurrentPentomino()->getType() == PENTOMINO_X)
    {
        //X pentominoes have the same initial and final states; appear to rotate.
        return true;
    }

    bool canRotate = false;

    //Copy the current pentomino's blocks' positions and store them temporarily.
    int n = this->getCurrentPentomino()->getN();
    int x[n], y[n];
    int dx[n], dy[n];
    int px[n], py[n];
    char ri = this->getCurrentPentomino()->getOrientation()%4;
    char rf = (ri+3)%4;
    int kx = 0, ky = 0;

    //Store the initial positions.
    for(int i = 0; i < n; i++)
    {
        x[i] = this->getCurrentPentomino()->getBlock(i)->getX();
        y[i] = this->getCurrentPentomino()->getBlock(i)->getY();
        dx[i] = this->getCurrentPentomino()->getBlock(i)->getDx();
        dy[i] = this->getCurrentPentomino()->getBlock(i)->getDy();
        px[i] = x[i]+dx[i];
        py[i] = y[i]+dy[i];
    }


    for(int k = 0; k < 5; k++) //Try to rotate, and then try 4 different kickoffs plus rotation.
    {
        for(int i = 0; i < n; i++)
        {
            //Add in possible translations from kicking off walls.
            px[i] = x[i]+dx[i]+kx;
            py[i] = y[i]+dy[i]+ky;

            if(!px[i] && !py[i])
            {
                continue;   //This block rotates on itself and will not move; disregard.
            }

            //Block will possibly rotate out of bounds.
            if(px[i]<0 || px[i]>=UTIL_GRID_WIDTH || py[i]>=UTIL_GRID_HEIGHT)
            {
                canRotate = false;
                break;  //Block will rotate out of bounds; abort current rotation.
            }

            if(this->getCell(px[i],py[i]) != nullptr)
            {
                //There -is- a block directly where this block should go. Is it a dead block?
                canRotate = false;
                if(this->getCell(px[i],py[i])->getLiveSwitch())
                {
                    canRotate = true; //This block is still live and will also be moved; disregard.
                }
                else
                {
                    break;  //Dead block detected; abort current rotation.
                }
            }
        }
        if(canRotate)
        {
            break; //Rotation can proceed with current kick off.
        }
        else
        {
            //Try to kick off the dead blocks.
            std::pair<int,int> p = this->getCurrentPentomino()->pullKickoffData(k+1,ri,rf);
            kx = p.first;
            ky = p.second;
        }
    }

    if(canRotate)
    {
        for(int i = 0; i < n; i++)
        {
            //Erase this block off the board.
            this->setCell(x[i],y[i],nullptr);

            //Do it!
            this->getCurrentPentomino()->getBlock(i)->setX(px[i]);
            this->getCurrentPentomino()->getBlock(i)->setY(py[i]);
            this->getCurrentPentomino()->getBlock(i)->setDx(dy[i]);
            this->getCurrentPentomino()->getBlock(i)->setDy(-dx[i]);
        }

        for(int j = 0; j < n; j++)
        {
            //Paste the rotated and translated block on the board.
            this->setCell(this->getCurrentPentomino()->getBlock(j)->getX(),
                          this->getCurrentPentomino()->getBlock(j)->getY(),
                          this->getCurrentPentomino()->getBlock(j));
            //Update orientation data.
            this->getCurrentPentomino()->setOrientation(rf);
        }

        //Find the shadow.
        this->findShadow();
    }
    return canRotate;
}

void Board::findShadow()
{
    int shadow = 0;
    Pentomino* p = this->getCurrentPentomino();

    while(shadow <= UTIL_GRID_HEIGHT)
    {
        for(int i = 0; i < p->getN(); i++)
        {
            char x = p->getBlock(i)->getX();
            char y = p->getBlock(i)->getY() + shadow;

            if(y>=UTIL_GRID_HEIGHT)
            {
                //Bottom row reached. Place shadow here.
                this->getCurrentPentomino()->setShadow(shadow-1);
                return;
            }

            if(this->getCell(x,y+1) != nullptr)
            {
                //There -is- a block directly under this one. Is it a dead block?
                if(this->getCell(x,y+1)->getLiveSwitch())
                {
                    //The block under of this one still live and will also move down; disregard.
                    continue;
                }
                else
                {
                    //This pentomino can no longer be moved downward. Place shadow here.
                    this->getCurrentPentomino()->setShadow(shadow);

                    //However, some other blocks' shadows might still end up below the floor in rare cases.
                    for(int j = i+1; j < p->getN(); j++)
                    {
                        if(p->getBlock(j)->getY() + shadow >= UTIL_GRID_HEIGHT)
                        {
                            this->getCurrentPentomino()->setShadow(shadow-1);
                            return;
                        }
                    }
                    return;
                }
            }
        }

        //Try the next line.
        shadow++;
    }

    return;
}


//Game over. Fill the board with black blocks.
bool Board::killBoard()
{
    unsigned char r = 0, g = 0, b = 0;
    if(this->getCell(0,0) != nullptr)
    {
        al_unmap_rgb(this->getCell(0,0)->getTint(),&r,&g,&b);
        if(r == 30 && g == 30 && b == 30)
        {
            //Complete board blacked out. Abort.
            return false;
        }
    }

    //Find the first row not yet blacked out.
    for(int j = UTIL_GRID_HEIGHT-1; j >= 0; j--)
    {
        r = 0;
        g = 0;
        b = 0;

        if(this->getCell(0,j) != nullptr)
        {
            al_unmap_rgb(this->getCell(0,j)->getTint(),&r,&g,&b);
            if(r == 30 && g == 30 && b == 30)
            {
                //This row is blacked out. Continue search.
                continue;
            }
        }

        //Black out this row.
        for(int i = 0; i < UTIL_GRID_WIDTH; i++)
        {
            if(this->getCell(i,j) != nullptr)
            {
                //Dead block exists. Kill it.
                //Much, much faster to just change the color.
                this->getCell(i,j)->setTint(al_map_rgb(30,30,30));
            }
            else
            {
                //Put a null block here.
                this->setCell(i,j,new Block(al_map_rgb(30,30,30)));
            }
        }
        return true;
    }

    return false;
}
