#include "../olcPixelgameEngine.h"


#ifdef IDEPLS
#include "main.cpp"
#endif


/*
custom blending functiion for palettes
    -paletteBlend

functions related to drawing or doing actions on the window
    -DrawSelectbox
    -(DrawStringSafe)
    -UpdateBankGFX
    -UpdateMapGFX

other functions not related to user input
    -getTiles
*/


/************************************************************************************/



/**
 * custom blending mode that applies the palette to the pixel
 *  --paletted value is the low half of the RED byte
 *  --color value must be in [[0x00000000, 0x0000000F]]
 *  --binary alpha blening is applied afterwards
*/
olc::Pixel paletteBlend(const int _x, const int _y, const olc::Pixel& src, const olc::Pixel& dest)
{
    //color 0 is always transparent
    if ((src.r & 0x0F) == 0x00) return {0, 0, 0};

    //else
    olc::Pixel* pal = state::palette->getPalette(blend::pal & 0x0F);
    olc::Pixel res = pal[src.r & 0x0F];

    //apply priority overlay
    if (blend::doPrioOverlay)
    {
        const olc::Pixel ov(0, 255, 0, 128);
        const float alpha = ov.a / 255.0F;
        res.r = alpha*ov.r + (1-alpha)*res.r;
        res.g = alpha*ov.g + (1-alpha)*res.g;
        res.b = alpha*ov.b + (1-alpha)*res.b;
    }

    return res;
}



/************************************************************************************/



/**
 * routine to update the bank tiles gfx
*/
void UpdateBankGFX(window* win) {
    win->SetPixelMode(paletteBlend);

    const olc::vi2d start = {pad::left+32*8+pad::center, pad::top};
    
    for (int i=0; i<0x200; i++)
{
        olc::vi2d pos = {i%16, i/16};
        
        blend::pal = state::selectedPalette;
        win->DrawSprite(start + 8*pos, state::tileData->data[i+0x200*state::gfxPage]);
    }

    win->SetPixelMode(olc::Pixel::MASK);
}




/************************************************************************************/



/**
 * draw a selection box at a position in pixels after clearing relvant screen areas
*/
void DrawSelectBox(window* win)
{
    olc::vi2d b_start  = state::selBox.first;
    olc::vi2d b_end = state::selBox.second;

    win->SetDrawTarget((uint8_t)0);
    win->SetPixelMode(olc::Pixel::NORMAL);

    win->FillRect(cst::tileStart, {16*8, 32*8}, olc::TRS);
    win->FillRect(cst::mapStart,  {32*8, 32*8}, olc::TRS);

    olc::Sprite* sp = &state::spriteMap[sprite::selbox];

    for (int i=0; i<(b_end.x-b_start.x)/4; i++) {
        win->DrawPartialSprite({b_start.x+i*4, b_start.y}, sp, {4,0}, {4,4});
        win->DrawPartialSprite({b_start.x+i*4, b_end.y-4}, sp, {4,8}, {4,4});
    }
    for (int i=0; i<(b_end.y-b_start.y)/4; i++) {
        win->DrawPartialSprite({b_start.x, b_start.y+i*4}, sp, {0,4}, {4,4});
        win->DrawPartialSprite({b_end.x-4, b_start.y+i*4}, sp, {8,4}, {4,4});
    }

    win->DrawPartialSprite(b_start, sp, {0,0}, {4,4});
    win->DrawPartialSprite(b_end-olc::vi2d(4,4), sp, {8,8}, {4,4});
    win->DrawPartialSprite({b_start.x, b_end.y-4}, sp, {0,8}, {4,4});
    win->DrawPartialSprite({b_end.x-4, b_start.y}, sp, {8,0}, {4,4});

    win->SetPixelMode(olc::Pixel::MASK);
    win->SetDrawTarget((uint8_t)1);
}




/************************************************************************************/




/**
 * @brief get the currently selected tiles
 * @return a [new] allocated pointer to the tiles
*/
snes::tiles* getTiles()
{
    //selection is in the edited tilemap
    if (state::selMode == mode::map)
    {
        olc::vi2d pad = {pad::left, pad::top};
        return state::currentMap->getTiles(state::selBox.first - pad, state::selBox.second - pad);
    }
    //senection is in the tile bank
    else if (state::selMode == mode::bank)
    {
        olc::vi2d pad = {pad::left + 32*8 + pad::center, pad::top};
        olc::vi2d start_tile = (state::selBox.first - pad) / 8;
        olc::vi2d end_tile = (state::selBox.second - pad) / 8;
        olc::vi2d size = {end_tile.x-start_tile.x, end_tile.y-start_tile.y};

        snes::tiles* res = new snes::tiles(size);

        for (int x=0; x<size.x; x++)
        for (int y=0; y<size.y; y++)
        {
            snes::tile nt;
                nt.id      = (start_tile.x+x) + (start_tile.y+y) * 16 + 0x200*state::gfxPage;
                nt.flip    = snes::axis::oo;
                nt.palette = state::selectedPalette;
            
            res->get({x,y}) = nt;
        }
        return res;

    }
    //if nothing is selected
    else
    {
        return new snes::tiles({0, 0});
    }
}




/************************************************************************************/



/**
 * draw a string withing a char limit (unused)
*/
void DrawStringSafe(window* win, std::string& src, olc::vi2d at, olc::Pixel color, int maxWidth) {
    std::string real = src;
    
    if (win->GetTextSizeProp(real).x > maxWidth) {
        real = "..." + real.substr(1);
        while (win->GetTextSizeProp(real).x > maxWidth)
            real = "..." + real.substr(4);
    }
    win->DrawStringProp(at, real, color);
}



/************************************************************************************/



/**
 * update the map tiles gfx 
*/
void UpdateMapGFX(window* win)
{
    win->SetPixelMode(paletteBlend);

    for (int x=0; x<snes::tilemap::sx; x++)
    for (int y=0; y<snes::tilemap::sy; y++)
    {
        snes::tile t = state::currentMap->get({x,y});
        if (t.id == -1) t = state::currentMap->defTile;

        olc::Sprite* gfx = state::tileData->get(t.id);
        blend::pal = t.palette;

        blend::doPrioOverlay = state::showPrioToggle && t.prio;

        win->DrawSprite({8*x + pad::left, 8*y + pad::top}, gfx, 1, t.flip);
    }

    blend::doPrioOverlay = false;

    win->SetPixelMode(olc::Pixel::MASK);
}


/************************************************************************************/

/**
 * @brief checks and corrects the selbox position
 * @return [true] -> selection success | [false] -> selection error
*/
bool try_update_selbox(int mx, int my)
{
    //round up 1 tile
    int x = mx - (mx % 8) + 8;
    int y = my - (my % 8) + 8;
    state::selBox.second = {x, y};

    if ((state::selBox.first.x != state::selBox.second.x)
     && (state::selBox.first.y != state::selBox.second.y))
    {
        state::selBox = {
            {std::min(state::selBox.first.x, state::selBox.second.x),
                std::min(state::selBox.first.y, state::selBox.second.y)},
            {std::max(state::selBox.first.x, state::selBox.second.x),
                std::max(state::selBox.first.y, state::selBox.second.y)},
        };

        return true;
    } else {
        return false;
    }
}