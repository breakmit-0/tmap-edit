#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "SNES.cpp"


#include <iostream>
#include <map>

snes::gfx* tileData;
snes::palette* palette;
uint8_t paletteSelector;
olc::EffArray* eff;
std::pair<olc::vi2d, olc::vi2d> selBox;
//snes::tilemap currentMap;

//size of the padding areas between the displays
enum padding {
    left = 16,
    center = 16,
    right = 16,
    bottom = 16,
    top = 40,
};

enum button {
    empty,
    open,
    save,
    flip_h,
    flip_v,
    down,
    text,
    cut,
    copy,
    paste,
    pal
};

enum sprites {
    selbox
};

enum actMode {
    none,
    selecting
}

actMode action = actMode::none;

struct mapEditor : public olc::PixelGameEngine
{
    const olc::vi2d mapStart = {padding::left, padding::top};
    const olc::vi2d tileStart = {padding::left + 32*8 + padding::center, padding::top};

    std::map<button, olc::Sprite> buttonMap;
    std::map<sprites, olc::Sprite> spriteMap;


    void DrawSelectBox(olc::v2d_generic<olc::vi2d> box) {
        SetDrawTarget((uint8_t)0);
        SetPixelMode(olc::Pixel::NORMAL);

        FillRect(tileStart, {16*8, 32*8}, olc::TRS);
        FillRect(mapStart,  {32*8, 32*8}, olc::TRS);

        for (int i=0; i<(box.y.x-box.x.x)/4; i++) {
            DrawPartialSprite({box.x.x+i*4, box.x.y}, &spriteMap[sprites::selbox], {4,0}, {4,4});
            DrawPartialSprite({box.x.x+i*4, box.y.y-4}, &spriteMap[sprites::selbox], {4,8}, {4,4});
        }
        for (int i=0; i<(box.y.y-box.x.y)/4; i++) {
            DrawPartialSprite({box.x.x, box.x.y+i*4}, &spriteMap[sprites::selbox], {0,4}, {4,4});
            DrawPartialSprite({box.y.x-4, box.x.y+i*4}, &spriteMap[sprites::selbox], {8,4}, {4,4});
        }

        DrawPartialSprite(box.x, &spriteMap[sprites::selbox], {0,0}, {4,4});
        DrawPartialSprite(box.y-olc::vi2d(4,4), &spriteMap[sprites::selbox], {8,8}, {4,4});
        DrawPartialSprite({box.x.x, box.y.y-4}, &spriteMap[sprites::selbox], {0,8}, {4,4});
        DrawPartialSprite({box.y.x-4, box.x.y}, &spriteMap[sprites::selbox], {8,0}, {4,4});

        SetPixelMode(olc::Pixel::MASK);
        SetDrawTarget((uint8_t)1);
    }

    void doEffects(float delta, olc::EffArray* effects) {
        SetDrawTarget((uint8_t)0); // 0 could be a nullptr for a olc::Sprite
        SetPixelMode(olc::Pixel::NORMAL);
        std::vector<size_t> toDelete = std::vector<size_t>();

        for (size_t i=0; i<effects->size(); i++) {
            (*effects)[i].first -= delta;
            if ((*effects)[i].first < 0) {
                toDelete.push_back(i);
                FillRect((*effects)[i].second.first, (*effects)[i].second.second, olc::TRS);
                std::cout << "disable\n";
                continue;
            }
            else {
                FillRect((*effects)[i].second.first, (*effects)[i].second.second, olc::WHITE);
            }
        }
        //iterate elems in revers order
        for ((void)0; toDelete.size() > 0; toDelete.pop_back()) {
            size_t elem = toDelete[(toDelete.size()-1)];
            (*effects).erase(effects->begin() + elem);
        }
        SetPixelMode(olc::Pixel::MASK);
        SetDrawTarget(1);
    }


    void DrawStringPropSafe(std::string& src, olc::vi2d at, olc::Pixel color, int maxWidth) {
        std::string real = src;
        
        if (GetTextSizeProp(real).x > maxWidth) {
            real = "..." + real.substr(1);
            while (GetTextSizeProp(real).x > maxWidth)
                real = "..." + real.substr(4);
        }
        DrawStringProp(at, real, color);
    }

    void UpdateGFXtiles(int offset, int count) {
        SetPixelMode(mapEditor::paletteBlend);

        const olc::vi2d start = {padding::left+32*8+padding::center, padding::top};
        for (int i=0; i<count; i++) {
            olc::vi2d pos = {(i+offset)%16, (i+offset)/16};
            DrawSprite(start + 8*pos, tileData->data[i+offset]);
        }

        SetPixelMode(olc::Pixel::MASK);
    }

    static olc::Pixel paletteBlend(const int _x, const int _y, const olc::Pixel& src, const olc::Pixel& dest) {
        //custom blening mode that applies the palette to the pixel
        //paletted value is the low half of the RED byte, so color in ABGR is the value
        //color value must be in [[0x00000000, 0x0000000F]]
        //binary alpha blening is applied afterwards

        //color 0 is always transparent
        if ((src.r & 0x0F) == 0x00) return dest;

        //else
        olc::Pixel* pal = palette->getPalette(paletteSelector & 0x0F);
        return pal[src.r & 0x0F];
    }

    bool OnUserCreate() override {
        tileData = new snes::gfx();
        palette = new snes::palette();
        paletteSelector = 0;
        eff = new olc::EffArray();

        SetPixelMode(olc::Pixel::MASK);
        Clear(olc::TRS);


        //default draws to layer 1
        CreateLayer();
        EnableLayer(1, true);
        SetDrawTarget(1);


        const olc::Pixel BGcolor = olc::Pixel(0xA0,0xA0,0xA0);
        const olc::Pixel BorderColor = olc::Pixel(0x40, 0x40, 0x40);

        FillRect({0, 0}, {ScreenWidth(), padding::top}, BGcolor);
        FillRect({0, 0}, {padding::left, ScreenHeight()}, BGcolor);
        FillRect({0, ScreenHeight()-padding::bottom}, {ScreenWidth(), padding::bottom}, BGcolor);
        FillRect({ScreenWidth()-padding::right, 0}, {padding::right, ScreenHeight()}, BGcolor);
        FillRect({padding::left+32*8, padding::top}, {padding::center, ScreenHeight()}, BGcolor);

        DrawRect({padding::left-1, padding::top-1}, {32*8+1, 32*8+1}, BorderColor);
        DrawRect({padding::left+32*8+padding::center-1, padding::top-1}, {16*8+1, 32*8+1}, BorderColor);

        buttonMap[button::open] = olc::Sprite("./assets/open.png");
        buttonMap[button::save] = olc::Sprite("./assets/save.png");
        buttonMap[button::flip_h] = olc::Sprite("./assets/flip_h.png");
        buttonMap[button::flip_v] = olc::Sprite("./assets/flip_v.png");
        buttonMap[button::down] = olc::Sprite("./assets/page_down.png");
        buttonMap[button::text] = olc::Sprite("./assets/textbox.png");
        buttonMap[button::empty] = olc::Sprite("./assets/empty.png");
        buttonMap[button::copy] = olc::Sprite("./assets/copy.png");
        buttonMap[button::paste] = olc::Sprite("./assets/paste.png");
        buttonMap[button::cut] = olc::Sprite("./assets/cut.png");
        buttonMap[button::pal] = olc::Sprite("./assets/palselect.png");

        spriteMap[sprites::selbox] = olc::Sprite("./assets/selbox.png");


        DrawSprite(olc::vi2d(padding::left,0), &buttonMap[button::open]);
        DrawSprite(olc::vi2d(padding::left+16,0), &buttonMap[button::save]);

        DrawSprite(olc::vi2d(padding::left,16), &buttonMap[button::text]);
        DrawSprite(olc::vi2d(padding::left+32,0), &buttonMap[button::empty]);

        DrawSprite(olc::vi2d(padding::left+3*16,0), &buttonMap[button::copy]);
        DrawSprite(olc::vi2d(padding::left+4*16,0), &buttonMap[button::paste]);
        DrawSprite(olc::vi2d(padding::left+5*16,0), &buttonMap[button::cut]);

        DrawSprite(olc::vi2d(padding::left+6*16,0), &buttonMap[button::flip_v]);
        DrawSprite(olc::vi2d(padding::left+7*16,0), &buttonMap[button::flip_h]);

        DrawSprite(olc::vi2d(padding::left+8*16,0), &buttonMap[button::pal]);

        DrawSprite(olc::vi2d(padding::left+32*8+padding::center,0), &buttonMap[button::open]);
        DrawSprite(olc::vi2d(padding::left+32*8+padding::center+16,0), &buttonMap[button::down]);
        DrawSprite(olc::vi2d(padding::left+32*8+padding::center,16), &buttonMap[button::text]);


        palette->load_pal("data/2.pal");
        paletteSelector = 0xA;

        int count = tileData->load("data/1.bin", 0x000);
            UpdateGFXtiles(0, count);


        DrawSelectBox({{tileStart}, {tileStart+olc::vi2d(16,16)}});


        return true;
    }

    bool OnUserUpdate(float delta) override {
        
        doEffects(delta, eff);

        

        if (GetMouse(0).bPressed) {
            int x = GetMouseX();
            int y = GetMouseY();

            const int palbounds[4] = {padding::left + 8*16,0,padding::left + 8*16 + 128,32};

            if (x>palbounds[0] && y>palbounds[1] && x<palbounds[2] && y<palbounds[3]){
                //get per tile coordinates
                x = (x-palbounds[0]) / 16;
                y = (y-palbounds[1]) / 16;

                paletteSelector = x+8*y;
                UpdateGFXtiles(0, 0x200); //temporary, should be a UpdateAllGFX call
                
                int xpos = 9*16 + x*16;
                int ypos = y*16;

                eff->push_back({0.1, {{xpos,ypos}, {16,16}}});
                
                return true;
            }


        }

        if (action==actMode::selecting && GetMouse(0).bReleased) {
            
        }

        /*
        if (*(int*)&_d & 1) {
            EnableLayer(0, true);
        } else {
            EnableLayer(0, false);
        }*/

        return true;
    }

    bool OnUserDestroy() override {
        delete tileData;
        delete palette;
        return true;
    }
};


int main() {
    mapEditor win;
    if (win.Construct(
        padding::left + 8*32 + padding::center + 8*16 + padding::right,
        padding::top + 8*32 + padding::bottom,
        2, 2, false, false, false
    )) {
        win.Start();
    }
    return 0;
}