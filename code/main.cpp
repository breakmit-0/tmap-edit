//replace with VK_OEM_3 for qwerty
#define VVK_BEFORE_1 VK_OEM_7
#define NEW_BACKUP_FILE "backup_new.tmap"
#define LOG_FILE "app.log"
#define ICON_NAME "icon.ico"
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"



/* if vscode works this file is included first
 * else, all other files have IDEPLS and inclde main
*/
#undef IDEPLS

//std includes
#include <iostream>
#include <map>

//define name
struct window : public olc::PixelGameEngine {
    bool OnUserCreate() override;
    bool OnUserUpdate(float) override;
    bool OnUserDestroy() override;
};

//file includes & globals
#include "logger.cpp"
#include "enums.cpp"
#include "openfile.cpp"
#include "proc/utils.cpp"
#include "datatypes.cpp"
#include "globals.cpp"
#include "proc/window.cpp"
#include "proc/action.cpp"

//entry point
int main(int argc, char** argv)
{
    state::argc = argc;
    state::argv = argv;

    //get file deirectory
    char path_tmp[MAX_PATH];
    GetModuleFileNameA(NULL, path_tmp, MAX_PATH);
    state::EXE_DIR = std::string(path_tmp);
    while(state::EXE_DIR.back() != '/' && state::EXE_DIR.back() != '\\') {
        state::EXE_DIR.pop_back();
        if (state::EXE_DIR.length() <= 0) {
            log_console << "[ERROR] <> app stared from an invalid loaction ?! : " << path_tmp;
            return 0;
        }
    }

    //setup logger
    log_console.stream.open(state::EXE_DIR+std::string(LOG_FILE), std::ios::trunc);
    
    //initialize constants
    cst::init_maps();

    //start with new file
    *state::editedFile = '\0';

    //startup meesage
    log_console << "\n--APP START--\n";
    log_console << "[INFO] <> executing from : " << state::EXE_DIR << "\n";


    window win;
    if (win.Construct(
        pad::left + 8*32 + pad::center + 8*16 + pad::right,
        pad::top + 8*32 + pad::bottom,
        2,2,   //px size
        false, //fullscreen
        false, //vsync
        false  //cohesion
    )) 
    {win.Start();}
    
    else {
        log_console << "startup error!!";
    }

    return 0;
}

//init
bool window::OnUserCreate()
{
    //init the global state
    state::tileData = new snes::gfx();
    state::palette = new snes::palette();
    state::currentMap = new snes::tilemap();
    state::clipboard = new snes::tiles({0, 0});
    state::pack = new olc::ResourcePack();
    state::selectedPalette = 0;
    state::selMode = mode::none;
    state::selBox = {{0,0},{0,0}};
    state::selBox_old = state::selBox;
    /*state::eff = new olc::EffArray();*/

    //init display
    SetPixelMode(olc::Pixel::MASK);
    Clear(olc::TRS);

    //use l0 for effects and default draws to l1
    CreateLayer();
    EnableLayer(1, true);
    SetDrawTarget(1);

    //setup background image
    FillRect({0, 0}, {ScreenWidth(), pad::top}, cst::BGcolor);
    FillRect({0, 0}, {pad::left, ScreenHeight()}, cst::BGcolor);
    FillRect({0, ScreenHeight()-pad::bottom}, {ScreenWidth(), pad::bottom}, cst::BGcolor);
    FillRect({ScreenWidth()-pad::right, 0}, {pad::right, ScreenHeight()}, cst::BGcolor);
    FillRect({pad::left+32*8, pad::top}, {pad::center, ScreenHeight()}, cst::BGcolor);
    DrawRect({pad::left-1, pad::top-1}, {32*8+1, 32*8+1}, cst::BorderColor);
    DrawRect({pad::left+32*8+pad::center-1, pad::top-1}, {16*8+1, 32*8+1}, cst::BorderColor);

    //load the UI sprites
    state::pack->LoadPack(state::EXE_DIR+"assets.pck", "assets-v1.0");
    state::buttonMap[butt::sprite::open]        = olc::Sprite("./open.png", state::pack);
    state::buttonMap[butt::sprite::newfile]     = olc::Sprite("./new.png", state::pack);
    state::buttonMap[butt::sprite::save]        = olc::Sprite("./save.png", state::pack);
    state::buttonMap[butt::sprite::saveas]      = olc::Sprite("./saveas.png", state::pack);
    state::buttonMap[butt::sprite::config]      = olc::Sprite("./config.png", state::pack);
    state::buttonMap[butt::sprite::fliph]       = olc::Sprite("./flip_h.png", state::pack);
    state::buttonMap[butt::sprite::flipv]       = olc::Sprite("./flip_v.png", state::pack);
    state::buttonMap[butt::sprite::page_down]   = olc::Sprite("./page_down.png", state::pack);
    state::buttonMap[butt::sprite::page_up]     = olc::Sprite("./page_up.png", state::pack);
    state::buttonMap[butt::sprite::fill1]       = olc::Sprite("./filler1.png", state::pack);
    state::buttonMap[butt::sprite::fill2]       = olc::Sprite("./filler2.png", state::pack);
    state::buttonMap[butt::sprite::empty]       = olc::Sprite("./empty.png", state::pack);
    state::buttonMap[butt::sprite::copy]        = olc::Sprite("./copy.png", state::pack);
    state::buttonMap[butt::sprite::paste]       = olc::Sprite("./paste.png", state::pack);
    state::buttonMap[butt::sprite::cut]         = olc::Sprite("./cut.png", state::pack);
    state::buttonMap[butt::sprite::prio]        = olc::Sprite("./prio.png", state::pack);
    state::buttonMap[butt::sprite::prio2]       = olc::Sprite("./prio2.png", state::pack);
    state::buttonMap[butt::sprite::tile0]       = olc::Sprite("./tile0.png", state::pack);
    state::buttonMap[butt::sprite::palmap]      = olc::Sprite("./palselect.png", state::pack);
    state::buttonMap[butt::sprite::gfx0]        = olc::Sprite("./load0.png", state::pack);
    state::buttonMap[butt::sprite::gfx1]        = olc::Sprite("./load1.png", state::pack);
    state::buttonMap[butt::sprite::gfx2]        = olc::Sprite("./load2.png", state::pack);
    state::buttonMap[butt::sprite::gfx3]        = olc::Sprite("./load3.png", state::pack);
    state::buttonMap[butt::sprite::pal]         = olc::Sprite("./loadpal.png", state::pack);
    state::buttonMap[butt::sprite::prio_show]   = olc::Sprite("./prio_show.png", state::pack);
    state::buttonMap[butt::sprite::prio_noshow] = olc::Sprite("./prio_noshow.png", state::pack);
    state::spriteMap[sprite::selbox]            = olc::Sprite("./selbox.png", state::pack);
    state::spriteMap[sprite::hover]             = olc::Sprite("./hover.png", state::pack);
    state::spriteMap[sprite::effect8x]          = olc::Sprite("./effect8x.png", state::pack);

    //display toolbar buttons
    DrawSprite(olc::vi2d(pad::left+0*16,0), &state::buttonMap[butt::sprite::newfile]);
    DrawSprite(olc::vi2d(pad::left+1*16,0), &state::buttonMap[butt::sprite::open]);
    DrawSprite(olc::vi2d(pad::left+2*16,0), &state::buttonMap[butt::sprite::save]);
    DrawSprite(olc::vi2d(pad::left+3*16,0), &state::buttonMap[butt::sprite::saveas]);

    DrawSprite(olc::vi2d(pad::left+0*16,16), &state::buttonMap[butt::sprite::config]);
    DrawSprite(olc::vi2d(pad::left+1*16,16), &state::buttonMap[butt::sprite::copy]);
    DrawSprite(olc::vi2d(pad::left+2*16,16), &state::buttonMap[butt::sprite::paste]);
    DrawSprite(olc::vi2d(pad::left+3*16,16), &state::buttonMap[butt::sprite::cut]);
    
    DrawSprite(olc::vi2d(pad::left+4*16,0), &state::buttonMap[butt::sprite::flipv]);
    DrawSprite(olc::vi2d(pad::left+5*16,0), &state::buttonMap[butt::sprite::fliph]);
    DrawSprite(olc::vi2d(pad::left+6*16,0), &state::buttonMap[butt::sprite::prio]);
    DrawSprite(olc::vi2d(pad::left+7*16,0), &state::buttonMap[butt::sprite::prio2]);

    DrawSprite(olc::vi2d(pad::left+4*16,16), &state::buttonMap[butt::sprite::fill1]);
    DrawSprite(olc::vi2d(pad::left+8*16,0), &state::buttonMap[butt::sprite::palmap]);
    DrawSprite(olc::vi2d(pad::left+15*16,16), &state::buttonMap[butt::sprite::pal]);
    
    //display altbar buttons
    DrawSprite(olc::vi2d(pad::left+32*8+pad::center+0*16,0), &state::buttonMap[butt::sprite::gfx0]);
    DrawSprite(olc::vi2d(pad::left+32*8+pad::center+1*16,0), &state::buttonMap[butt::sprite::gfx1]);
    DrawSprite(olc::vi2d(pad::left+32*8+pad::center+2*16,0), &state::buttonMap[butt::sprite::gfx2]);
    DrawSprite(olc::vi2d(pad::left+32*8+pad::center+3*16,0), &state::buttonMap[butt::sprite::gfx3]);
    DrawSprite(olc::vi2d(pad::left+32*8+pad::center+4*16,0), &state::buttonMap[butt::sprite::page_down]);
    DrawSprite(olc::vi2d(pad::left+32*8+pad::center+5*16,0), &state::buttonMap[butt::sprite::empty]);
    DrawSprite(olc::vi2d(pad::left+32*8+pad::center+6*16,0), &state::buttonMap[butt::sprite::tile0]);
    DrawSprite(olc::vi2d(pad::left+32*8+pad::center+7*16,0), &state::buttonMap[butt::sprite::prio_noshow]);
    DrawSprite(olc::vi2d(pad::left+32*8+pad::center+0*16,16), &state::buttonMap[butt::sprite::fill2]);


    //load the default palette
    state::palette->load_pal(state::EXE_DIR+"default.pal");
    state::selectedPalette = 0x00;

    //load a tilemap if an argument is provided
    if (state::argc >= 2)
    {
        FILE* file = std::fopen(state::argv[1], "rb");
        uint8_t* raw = (uint8_t*)malloc(2*32*32);
        std::fread(raw, 1, 2*32*32, file);
        std::fclose(file);

        uint8_t lo, hi;

        std::strcpy(state::editedFile, state::argv[1]);
        state::isSaved = true;

        log_console << "[INFO] <> opening 'open with' tilemap: " << state::argv[1] << "\n";

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

    //draw default bank/map
    UpdateBankGFX(this);
    UpdateMapGFX(this);   
    return true;
};


//cleanup
bool window::OnUserDestroy() {
    delete state::tileData;
    delete state::palette;
    delete state::currentMap;
    delete state::clipboard;
    delete state::pack;
    return true;
};


//mainloop
bool window::OnUserUpdate(float delta)
{
    int mx = GetMouseX();
    int my = GetMouseY();
    int tx = (mx - pad::left) / 8;
    int ty = (my - pad::top)  / 8;

    /*********************************************************************/

    if (*state::editedFile == '\0') {
        sEditedFile = "New File";
    } else {
        sEditedFile = std::string(state::editedFile) + std::string(state::isSaved ? "":" (Unsaved Changes)");
    }
    
    /**************************** KEYBINDS *******************************/


    //1-keybinds
    if (GetKey(olc::Key::DEL).bPressed) {
        act::del(this);
        state::doGFXupdate = true;
    }

    //ctrl-keybinds
    if (GetKey(olc::Key::CTRL).bHeld)
    {
        if (GetKey(olc::Key::C).bPressed) act::copy(this);
        if (GetKey(olc::Key::X).bPressed) act::cut(this);
        if (GetKey(olc::Key::V).bPressed) act::paste(this);

        if (GetKey(olc::Key::F).bPressed) {
            if (GetKey(olc::Key::SHIFT).bHeld)
                act::flipV(this);
            else
                act::flipH(this);
        }

        if (GetKey(olc::Key::P).bPressed) {
            if (GetAsyncKeyState(VK_MENU) & (0x8000))
                act::prio(this, -1);
            else {
                if (GetKey(olc::Key::SHIFT).bHeld)
                    act::prio(this, 0);
                else
                    act::prio(this, 1);
            }
        }

        if (GetKey(olc::Key::S).bPressed) {
            if (GetKey(olc::Key::SHIFT).bHeld)
                act::saveas(this);
            else
                act::save(this);
        }
        if (GetKey(olc::Key::O).bPressed) act::open(this);
        if (GetKey(olc::Key::N).bPressed) act::newfile(this);

        if (GetAsyncKeyState(VVK_BEFORE_1) & 0x8000) act::setpalette(this, 0);
        if (GetAsyncKeyState(0x30) & 0x8000) act::setpalette(this, 0);
        if (GetAsyncKeyState(0x31) & 0x8000) act::setpalette(this, 1);
        if (GetAsyncKeyState(0x32) & 0x8000) act::setpalette(this, 2);
        if (GetAsyncKeyState(0x33) & 0x8000) act::setpalette(this, 3);
        if (GetAsyncKeyState(0x34) & 0x8000) act::setpalette(this, 4);
        if (GetAsyncKeyState(0x35) & 0x8000) act::setpalette(this, 5);
        if (GetAsyncKeyState(0x36) & 0x8000) act::setpalette(this, 6);
        if (GetAsyncKeyState(0x37) & 0x8000) act::setpalette(this, 7);

        state::doGFXupdate = true;

    }


    /****************************** RIGHT CLICK ***************************/


    if (GetMouse(1).bPressed && is_in({mx,my}, bound::map))
    {
        state::isSaved = false;

        //print the selected tiles
        snes::tiles* select = getTiles();
        olc::vi2d selSize = {select->sx, select->sy};
        state::currentMap->pasteTiles({tx,ty}, select);
        delete select;


        int x = mx - (mx % 8);
        int y = my - (my % 8);

        //update the selected area
        if (selSize.x > 0 && selSize.y > 0)
        {
            state::selBox.first = {x, y};
            state::selBox.second = state::selBox.first + 8*selSize;
            log_console << selSize << "\n";

            state::selBox.second = clampUp(state::selBox.second, {bound::map[2], bound::map[3]});
        }
        //update other stuff
        state::selMode = mode::map;

        state::doGFXupdate = true;
    }


    /************************** HOVER EFFECT ********************************/

    if (!GetMouse(0).bPressed)
    {
        bool showHover = false;
        int x = mx/16;
        int y = my/16;

        //draw conditions
        if (is_in({mx,my}, bound::altbar)) {
            showHover = true;
        }
        if (is_in({mx,my}, bound::mainarea) && !is_in({mx,my}, bound::fill1)) {
            showHover = true;
        }

        //draw the hover hint over the button
        if (showHover) {
            SetDrawTarget((uint8_t)0);
            olc::Decal d = olc::Decal(&state::spriteMap[sprite::hover]);
            DrawDecal({(float)16*x, (float)16*y}, &d, {1,1}, olc::Pixel(255,255,255,128));
            SetDrawTarget((uint8_t)1);
        }
    }


    /***************************** BUTTON ACTIONS ***************************/


    if (GetMouse(0).bPressed)
    {
        if (is_in({mx,my}, bound::altbar))
        { 
            int x = (mx-bound::altbar[0])/16;
            int y = (my-bound::altbar[1])/16;
            
            log_console << "[INFO] <> altbar action #" << cst::altmap[{x,y}] << "\n";
            state::doGFXupdate = true;

            switch (cst::altmap[{x,y}])
            {
            case butt::alt::gfx0:     act::openGFX(this, 0 + 4*state::gfxPage); break;
            case butt::alt::gfx1:     act::openGFX(this, 1 + 4*state::gfxPage); break;
            case butt::alt::gfx2:     act::openGFX(this, 2 + 4*state::gfxPage); break;
            case butt::alt::gfx3:     act::openGFX(this, 3 + 4*state::gfxPage); break;
            case butt::alt::page:     act::pageSwap(this);                      break;
            case butt::alt::tile0:    act::tile0(this);                         break;
            case butt::alt::showprio: act::showprio(this);                      break;
            case butt::alt::empty: break;
            default: break;
            }
        }

        if (is_in({mx,my}, bound::mainbar))
        {
            int x = (mx-bound::mainbar[0])/16;
            int y = (my-bound::mainbar[1])/16;
            
            log_console << "[INFO] <> mainbar action #" << cst::toolmap[{x,y}] << "\n";
            state::doGFXupdate = true;

            switch (cst::toolmap[{x,y}])
            {
            case butt::main::newfile: act::newfile(this); break;
            case butt::main::open:    act::open(this);    break;
            case butt::main::save:    act::save(this);    break;
            case butt::main::saveas:  act::saveas(this);  break;
            case butt::main::config:  act::config(this);  break;
            case butt::main::copy:    act::copy(this);    break;
            case butt::main::cut:     act::cut(this);     break;
            case butt::main::paste:   act::paste(this);   break;
            case butt::main::fliph:   act::flipH(this);   break;
            case butt::main::flipv:   act::flipV(this);   break;
            case butt::main::prio:    act::prio(this, 1); break;
            case butt::main::prio2:   act::prio(this, 0); break;
            case butt::main::empty: break;
            default: break;              
            }
        }

        if (is_in({mx,my}, bound::loadpal))
        {
            state::doGFXupdate = true;
            act::openpal(this);
        }

        if (is_in({mx,my}, bound::pal))
        {
            int x = (mx-bound::pal[0]) / 16;

            state::doGFXupdate = true;
            act::setpalette(this, x);
        }
    }


    /************************** START SELECTION *********************************/


    if (GetMouse(0).bPressed)
    {
        if (is_in({mx,my}, bound::map) && state::selPhase == phase::none)
        {
            state::selPhase = phase::map;

            int x = mx - (mx % 8);
            int y = my - (my % 8);
            state::selBox.first = {x, y};
        }
        if (is_in({mx,my}, bound::tile) && state::selPhase == phase::none)
        { 
            state::selPhase = phase::bank;

            //round down to a block
            int x = mx - (mx % 8);
            int y = my - (my % 8);
            state::selBox.first = {x, y};
        }
    }


    /********************** END SELECTION : MAP ***************************/



    if (GetMouse(0).bReleased && state::selPhase == phase::map)
    {
        state::selPhase = phase::none;

        if (is_in({mx,my}, bound::map))
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
                state::selBox_old = state::selBox;
                state::selMode = mode::map;
            } else {
                state::selBox = state::selBox_old;
            }
        } else {
            state::selBox = state::selBox_old;
        }
        state::doGFXupdate = true;
    }


    /***************************** END SELECTION : BANK TILES *******************************/


    if (GetMouse(0).bReleased && state::selPhase == phase::bank)
    {
        state::selPhase = phase::none;
        
        if (is_in({mx,my}, bound::tile) && try_update_selbox(mx, my))
        {
            state::selBox_old = state::selBox;
            state::selMode = mode::bank;
        }
        else {
            state::selBox = state::selBox_old;
        }

        state::doGFXupdate = true;
    }


    if (state::doGFXupdate) {
        UpdateMapGFX(this);
        DrawSelectBox(this);
    state::doGFXupdate = false;
    }

    return true;
};
