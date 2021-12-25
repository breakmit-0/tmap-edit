#include "olcPixelGameEngine.h"


#ifdef IDEPLS
#include "main.cpp"
#endif


//global state
namespace state
{
    //main edited object
    snes::tilemap* currentMap;

    //side loaded data
    snes::gfx* tileData;
    snes::palette* palette;

    //selection box
    std::pair<olc::vi2d, olc::vi2d> selBox;
    std::pair<olc::vi2d, olc::vi2d> selBox_old;

    //selected palette
    uint8_t selectedPalette;

    //effect stack TODO: make a better system fo that
    /*olc::EffArray* eff;*/
    
    //tile clipboard
    snes::tiles* clipboard;

    //selection phase
    phase::name selPhase = phase::none;
    
    //currently selected area (none|map|bank)
    mode::name selMode = mode::none;
    
    //gfx do display: 0->[0;3], 1->[4;7]
    int gfxPage = 0;

    //UI sprite containers
    std::map<butt::sprite::name, olc::Sprite> buttonMap;
    std::map<sprite::name, olc::Sprite> spriteMap;

    //edited file ('\0' if new file)
    char editedFile[MAX_PATH];
    bool isSaved = false;

    //show priority tiles switch
    bool showPrioToggle = false;

    //update gfx this frame
    bool doGFXupdate = true;

    //path to the folder of this executable
    std::string EXE_DIR;

    //assets
    olc::ResourcePack* pack;

    //global state copies
    int argc;
    char** argv;
};

//extra blending arguments
namespace blend {
    uint8_t pal;
    bool doPrioOverlay = false;
}

//constants
namespace pad {
    const int left = 16;
    const int right = 16;
    const int top = 40;
    const int bottom = 16;
    const int center = 16;
};
namespace olc {
    const olc::Pixel TRS = olc::Pixel(0, 0, 0, 0);
};
namespace cst
{   
    const olc::Pixel BGcolor = olc::Pixel(0xA0,0xA0,0xA0);
    const olc::Pixel BorderColor = olc::Pixel(0x40, 0x40, 0x40);
    const olc::vi2d mapStart = {pad::left, pad::top};
    const olc::vi2d tileStart = {pad::left + 32*8 + pad::center, pad::top};

    std::map<olc::vi2d, butt::main::name> toolmap;
    std::map<olc::vi2d, butt::alt::name> altmap;
    void init_maps() {
        toolmap[{0,0}] = butt::main::newfile;
        toolmap[{1,0}] = butt::main::open;
        toolmap[{2,0}] = butt::main::save;
        toolmap[{3,0}] = butt::main::saveas;
        toolmap[{4,0}] = butt::main::flipv;
        toolmap[{5,0}] = butt::main::fliph;
        toolmap[{6,0}] = butt::main::prio;
        toolmap[{7,0}] = butt::main::prio2;
        toolmap[{0,1}] = butt::main::config;
        toolmap[{1,1}] = butt::main::copy;
        toolmap[{2,1}] = butt::main::cut;
        toolmap[{3,1}] = butt::main::paste;
        toolmap[{4,1}] = butt::main::empty;
        toolmap[{5,1}] = butt::main::empty;
        toolmap[{6,1}] = butt::main::empty;
        toolmap[{7,1}] = butt::main::empty;

        altmap[{0,0}] = butt::alt::gfx0;
        altmap[{1,0}] = butt::alt::gfx1;
        altmap[{2,0}] = butt::alt::gfx2;
        altmap[{3,0}] = butt::alt::gfx3;
        altmap[{4,0}] = butt::alt::page;
        altmap[{5,0}] = butt::alt::empty;
        altmap[{6,0}] = butt::alt::tile0;
        altmap[{7,0}] = butt::alt::showprio;
        altmap[{0,1}] = butt::alt::empty;
        altmap[{1,1}] = butt::alt::empty;
        altmap[{2,1}] = butt::alt::empty;
        altmap[{3,1}] = butt::alt::empty;
        altmap[{4,1}] = butt::alt::empty;
        altmap[{5,1}] = butt::alt::empty;
        altmap[{6,1}] = butt::alt::empty;
        altmap[{7,1}] = butt::alt::empty;

    }

};
namespace bound
{
    int map[4] =     {pad::left, pad::top, pad::left+32*8, pad::top+32*8};
    int tile[4] =    {pad::left+32*8+pad::center, pad::top, pad::left+(32+16)*8+pad::center, pad::top+32*8};
    
    int pal[4] =      {pad::left+8*16, 0, pad::left+16*16, 16};
    int mainbar[4] =  {pad::left, 0, pad::left+8*16, 32};
    int loadpal[4] =  {pad::left+15*16, 16, pad::left+16*16, 32};

    int mainarea[4] = {pad::left, 0, pad::left+16*16, 32};
    int fill1[4] = {pad::left+4*16, 16, pad::left+15*16, 32};
    
    int altbar[4] =  {pad::left+32*8+pad::center, 0, pad::left+32*8+pad::center+8*16, 16};
    int prioButt[4] = {altbar[0]+7*16, 0, altbar[8]+8*16, 16};
}