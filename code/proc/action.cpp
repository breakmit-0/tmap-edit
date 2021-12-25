#include "../olcPixelGameEngine.h"


#ifdef IDEPLS
#include "../main.cpp"
#endif

/*
responses to user input
    -act::open
    -act::save
    -act::saveas
    -act::del
    -act::copy
    -act::cut
    -act::paste
    -act::flipH
    -act::filpV
    -act::openGFX
    -act::swapPage
    -act::openPal
*/

namespace act {
    void newfile(window*);
    void open(window*);
    void save(window*);
    void saveas(window*);
    void config(window*);

    void del(window*);
    void setpalette(window*, int);

    void copy(window*);
    void cut(window*);
    void paste(window*);

    void flipH(window*);
    void flipV(window*);
    void prio(window*, int);
    void tile0(window*);


    void openGFX(window*, int);
    void pageSwap(window*);
    void showprio(window*);

    void openpal(window*);
};

/***************************************************************************************/


template<class T>
void saveMap(T fn)
{
    FILE* file = std::fopen(fn, "wb");
    uint8_t* raw = (uint8_t*)malloc(2*state::currentMap->sx*state::currentMap->sy);
    uint8_t lo = 0;
    uint8_t hi = 0;
    snes::tile t;

    for (int y=0; y<32; y++)
    for (int x=0; x<32; x++)
    {
        int idx = y * 32 + x;
        t = state::currentMap->get({x,y});
        if (t.id == -1) t = state::currentMap->defTile;
        
        lo = 0;
        lo = t.id & 0b11111111;
        
        hi = 0;
        hi |= (t.id >> 8) & 0b00000011;
        hi |= (t.palette & 0b0111) << 2;
        hi |= 0 << 5;
        hi |= t.flip << 6;

        raw[2*idx] = lo;
        raw[2*idx+1] = hi;
    }

    std::fwrite(raw, 1, 2*state::currentMap->sx*state::currentMap->sy, file);
    std::fclose(file);
    free(raw);
}



/***************************************************************************************/



void act::config(window* win)
{

}

/***************************************************************************************/


void act::openpal(window* win)
{
    char fn[MAX_PATH];
    getOpenFile(fn, "Open File", "Palette Files (*.pal)\0*.pal\0All Files (*.*)\0*.*\0");
    if (fn[0] == '\0') {return;}

    state::palette->load_pal(std::string(fn));
    UpdateBankGFX(win);
}




/***************************************************************************************/



void act::setpalette(window* win, int num)
{
    state::selectedPalette = num;

    //apply the palette to selection
    if (state::selMode == mode::map)
    {
        snes::tiles* selected = getTiles();
        
        //update
        for (int x=0; x<selected->sx; x++)
        for (int y=0; y<selected->sy; y++) {
            (*selected)[{x,y}].palette = state::selectedPalette; 
        }

        //and paste back
        olc::vi2d pos = state::selBox.first;
        pos.x = (pos.x - pad::left) / 8;
        pos.y = (pos.y - pad::top) / 8;
        state::currentMap->pasteTiles(pos, selected);

        delete selected;
    }
    UpdateBankGFX(win);
}



/***************************************************************************************/



void act::showprio(window* win)
{
    
    state::showPrioToggle ^= 1;

    olc::Sprite* bt = state::showPrioToggle 
        ? &state::buttonMap[butt::sprite::prio_show]
        : &state::buttonMap[butt::sprite::prio_noshow];

    win->DrawSprite(olc::vi2d(bound::altbar[0]+7*16,0), bt);
}



/***************************************************************************************/


void act::newfile(window* win)
{
    for (int y=0; y<state::currentMap->sy; y++)
    for (int x=0; x<state::currentMap->sx; x++) {
        state::currentMap->get({x,y}).id = -1;
    }

    saveMap<const char*>((state::EXE_DIR+NEW_BACKUP_FILE).c_str());

    *state::editedFile = '\0';
    state::isSaved = false;
}



/***************************************************************************************/



void act::tile0(window* win)
{
    if (state::selMode == mode::none) return;

    snes::tiles* selected = getTiles();
    snes::tile t = selected->get({0,0});
    if (t.id == -1) t = state::currentMap->defTile;

    //update actual tile0
    state::currentMap->defTile = t;

    //draw an indicator
    win->SetPixelMode(paletteBlend);
    olc::Sprite* gfx = state::tileData->get(t.id);
    blend::pal = t.palette;
    win->DrawSprite({bound::altbar[0]+6*16+7, 7}, gfx, 1, t.flip);
    win->SetPixelMode(olc::Pixel::MASK);

    delete selected;
}



/***************************************************************************************/



void act::prio(window* win, int val)
{
    if (state::selMode != mode::map) return;

    snes::tiles* selected = getTiles();
    olc::vi2d pos = state::selBox.first;
    pos.x = (pos.x - pad::left) / 8;
    pos.y = (pos.y - pad::top) / 8;

    //make changes
    for (int y=0; y<selected->sy; y++)
    for (int x=0; x<selected->sx; x++) {
        if (val == -1) 
            {selected->get({x,y}).prio ^= 1;}
        else
            {selected->get({x,y}).prio = val;}
    }
    state::currentMap->pasteTiles(pos, selected);

    state::isSaved = false;

    delete selected;
}



/***************************************************************************************/



void act::flipH(window* win)
{
    if (state::selMode != mode::map) return;
    
    snes::tiles* selected = getTiles();
    olc::vi2d pos = state::selBox.first;
    pos.x = (pos.x - pad::left) / 8;
    pos.y = (pos.y - pad::top) / 8;

    selected->fliph();
    state::currentMap->pasteTiles(pos, selected);

    state::isSaved = false;

    delete selected;
    return;
}



/***************************************************************************************/



void act::flipV(window* win)
{
    if (state::selMode != mode::map) return;

    snes::tiles* selected = getTiles();
    
    olc::vi2d pos = state::selBox.first;
    pos.x = (pos.x - pad::left) / 8;
    pos.y = (pos.y - pad::top) / 8;

    selected->flipv();
    state::currentMap->pasteTiles(pos, selected);
    state::isSaved = false;

    delete selected;
    return;
}



/***************************************************************************************/



void act::del(window* win)
{
    if (state::selMode != mode::map) return;
    
    snes::tiles* selected = getTiles();
    for (int x=0; x<selected->sx; x++)
    for (int y=0; y<selected->sy; y++) {
        (*selected)[{x,y}].id = -1; 
    }

    olc::vi2d pos = state::selBox.first;
    pos.x = (pos.x - pad::left) / 8;
    pos.y = (pos.y - pad::top) / 8;

    state::currentMap->pasteTiles(pos, selected);

    state::isSaved = false;

    delete selected;
    return;
}



/***************************************************************************************/



void act::copy(window* win)
{
    if (state::selMode == mode::none) return;

    //new-switcharoo the clipboard
    delete state::clipboard;
    state::clipboard = getTiles();

    return;
}



/***************************************************************************************/



void act::cut(window* win)
{
    act::copy(win);
    act::del(win);
    state::isSaved = false;
    return;
}



/***************************************************************************************/



void act::paste(window* win)
{
    if (state::selMode != mode::map) return;

    //non-owned reference, dont delete
    snes::tiles* selected = state::clipboard;

    olc::vi2d pos = state::selBox.first;
    pos.x = (pos.x - pad::left) / 8;
    pos.y = (pos.y - pad::top) / 8;

    state::currentMap->pasteTiles(pos, selected);


    //update the select box
    state::selBox.second = state::selBox.first + 8 * olc::vi2d(selected->sx, selected->sy);
    state::selBox.second = {std::min(state::selBox.second.x,pad::left+32*8), std::min(state::selBox.second.y,pad::top+32*8)};

    state::isSaved = false;

}



/***************************************************************************************/



void act::open(window* win)
{
    char fn[MAX_PATH];
    getOpenFile(fn, "Open File", "Tilemap Files (*.tmap;*.bin)\0*.tmap;*.bin\0All Files (*.*)\0*.*\0");
    if (fn[0] == '\0') {return;}

    FILE* file = std::fopen(fn, "rb");
    uint8_t* raw = (uint8_t*)malloc(2*state::currentMap->sx*state::currentMap->sy);
    std::fread(raw, 1, 2*state::currentMap->sx*state::currentMap->sy, file);
    std::fclose(file);

    uint8_t lo, hi;

    std::strcpy(state::editedFile, fn);
    state::isSaved = true;

    log_console << "[INFO] <> loading tilemap from <" << fn << ">\n";

    for (int y=0; y<32; y++)
    for (int x=0; x<32; x++)
    {
        int idx = pos2idx(state::currentMap->sx)({x,y});
        lo = raw[2*idx];
        hi = raw[2*idx+1];

        snes::tile t;
            t.id = lo + ((hi & 0b00000011) << 8);
            t.palette = ((hi & 0b00011100) >> 2);
            t.prio =    ((hi & 0b00100000) >> 5);
            t.flip =    ((hi & 0b11000000) >> 6);

        state::currentMap->get({x,y}) = t;
    }
    free(raw);
}


/***************************************************************************************/



void act::saveas(window* win)
{
    char fn[MAX_PATH];
    getSaveFile(fn, "Save As", "Tilemap Files (*.tmap;*.bin)\0*.tmap;*.bin\0All Files (*.*)\0*.*\0");
    if (fn[0] == '\0') {return;}

    strcpy(state::editedFile, fn);
    state::isSaved = true;
    log_console << "[INFO] <> saving tilemap to <" << fn << ">\n";
    saveMap<char*>(fn);
}



/***************************************************************************************/



void act::openGFX(window* win, int slot)
{
    //slot is in [0;7]
    slot *= 0x80;

    char fn[MAX_PATH];
    getOpenFile(fn, "Save As", "Graphics Files (*.gfx;*.bin)\0*.gfx;*.bin\0All Files (*.*)\0*.*\0");
    if (fn[0] == '\0') {return;}

    state::tileData->load(std::string(fn), slot);
        UpdateBankGFX(win);
    
}



/***************************************************************************************/



void act::pageSwap(window* win)
{
    state::gfxPage ^= 1;
    
    olc::Sprite* bt = state::gfxPage 
        ? &state::buttonMap[butt::sprite::page_up]
        : &state::buttonMap[butt::sprite::page_down];

    win->DrawSprite(olc::vi2d(pad::left+32*8+pad::center+4*16,0), bt);
    
    UpdateBankGFX(win);
}



/***************************************************************************************/



void act::save(window* win)
{

    if (state::editedFile[0] == '\0') {
        act::saveas(win);
        return;
    }

    state::isSaved = true;
    saveMap<char*>(state::editedFile);
    log_console << "[INFO] <> saving tilemap to <" << state::editedFile << ">\n";
}

