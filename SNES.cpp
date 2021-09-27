#include "olcPixelGameEngine.h"

#include "staticPGE.cpp"
#include <iostream>
#include <cstdio>

namespace snes {
    struct gfx;
    struct tilemap;
    struct palette;
}


//a spimple struct that contains 16 arrays of 16 colors that can be loaded from a 16bit snes palette
struct snes::palette {
    olc::Pixel data[0x100];

    //simple hi/lo byte 2d access
    olc::Pixel& operator[](int idx) {
        return data[idx];
    }

    //load from file
    void load_pal(std::string name) {
        FILE* file = std::fopen(name.c_str(), "rb");
        char* raw = (char*)malloc(3*256);
        std::fread(raw, 3, 256, file);
        std::fclose(file);

        for (int i=0; i<256; i++) {
            data[i] = olc::Pixel(raw[3*i], raw[3*i+1], raw[3*i+2], 0xFF);
            //std::cout << std::hex << (int)data[i].r << "-" << (int)data[i].g << "-" << (int)data[i].b << ((i+1)%16 ? " " : "\n");
        }
    }

    void load_spl(std::string name) {
        /*to implement*/
    }

    //get one of the 16 color palettes
    olc::Pixel* getPalette(int idx) {
        return data + 16*idx;
    }
};

struct snes::gfx {
    olc::Sprite* data[0x400]; //number of allowed tiles 

    gfx() {        
        for (int i=0; i<0x400; i++)
            data[i] = new olc::Sprite(8,8);
    }

    void init() {
    }

    ~gfx() {
        for (int i=0; i<0x400; i++)
            delete data[i];
    }


    int load(std::string name, int offset) {
        FILE* file = std::fopen(name.c_str(), "rb");
        std::fseek(file, 0, SEEK_END);
        int count = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);

        uint8_t* raw = (uint8_t*)malloc(count*sizeof(uint8_t));
        fread(raw, sizeof(uint8_t), count, file);

        uint8_t pixel;
        uint8_t bitmap[64];
        const int tileSize = 8*4;


        for(int i=0; i<count/tileSize; i++) { //count is guaranteed to be a multiple of tileSize
            for (int y=0; y<8; y++) {
            for (int x=0; x<8; x++) {
                pixel = 0;
                if (raw[tileSize*i + 2*y] & (0x80>>x)) pixel |= 0b0001;
                if (raw[tileSize*i + 2*y+1] & (0x80>>x)) pixel |= 0b0010;
                if (raw[tileSize*i + tileSize/2 + 2*y] & (0x80>>x)) pixel |= 0b0100;
                if (raw[tileSize*i + tileSize/2 + 2*y+1] & (0x80>>x)) pixel |= 0b1000;

                data[i+offset]->SetPixel({x,y}, olc::Pixel(pixel, 0, 0, 0xFF));
            }
            }
        }
        free(raw);
        fclose(file);
        return count/tileSize;
    }

    bool printTile(olc::Sprite &target, int tile, olc::vi2d at) {
        if (tile >= 0x400) return false;
        olc::DrawSprite(*data[tile], target, at);
        return true;
    }
};
