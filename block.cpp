#include "block.h"

Block::Block(ALLEGRO_COLOR clr)
{
    //Most of the block's data is loaded during pentomino construction.

    //Draw a transparent frame.
    ALLEGRO_BITMAP* bmp = al_create_bitmap(UTIL_BLOCK_SIZE, UTIL_BLOCK_SIZE);
    al_set_target_bitmap(bmp);
    al_clear_to_color(al_premul_rgba(0,0,0,0));

    //Draw the actual block within the frame.
    ALLEGRO_BITMAP* sub = al_create_sub_bitmap(bmp, 1, 1, UTIL_BLOCK_SIZE-2, UTIL_BLOCK_SIZE-2);
    al_set_target_bitmap(sub);
    al_clear_to_color(al_map_rgb(255,255,255));
    al_destroy_bitmap(sub);
    sub = nullptr;

    //Do it!
    this->setSprite(bmp);

    //Load a custom skin on the sprite.
    //this->setSprite(al_load_bitmap("block.png"));

    //Color the block based on its type.
    this->setTint(clr);
}

Block::~Block()
{
    //Destroy the sprites.
    al_destroy_bitmap(this->getSprite());
    this->setSprite(nullptr);
}
