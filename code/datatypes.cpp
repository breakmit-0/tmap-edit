#include "olcPixelGameEngine.h"

#ifdef IDEPLS
#include "main.cpp"
#endif


/*
---actual data holding types---

-tile
    1 tile: number + palette + flip

-gfx
    olc::Sprite array, indexed
    by tile numbers

-tiles
    generic 2d tile array, dynamically
    allocated so should not be copied

-tilemap
    main data structure, 2d
    array of tile

-palette
    16 x 16 colors loaded from a
    16bit snes palette
*/


namespace snes {
    struct tile;
    struct gfx;
    struct tiles;
    struct tilemap;
    struct palette;
}

/**************************************** snes::palette ****************************************/

struct snes::palette
{
    olc::Pixel data[128];

    //simple hi/lo byte 2d access
    olc::Pixel& operator[](int idx) {
        return data[idx];
    }

    //load from file (8-8-8 RGB)
    void load_pal(std::string name) {
        FILE* file = std::fopen(name.c_str(), "rb");
        char* raw = (char*)malloc(3*128);
        std::fread(raw, 3, 128, file);
        std::fclose(file);

        for (int i=0; i<128; i++) {
            data[i] = olc::Pixel(raw[3*i], raw[3*i+1], raw[3*i+2], 0xFF);
        }
    }

    //get one of the 16 color palettes
    olc::Pixel* getPalette(int idx) {
        return data + 16*idx;
    }
};




/**************************************** snes::gfx ****************************************/



struct snes::gfx {
    olc::Sprite* data[0x400]; //number of allowed tiles 
    olc::Sprite* empty;

    gfx() {        
        for (int i=0; i<0x400; i++)
            data[i] = new olc::Sprite(8,8);
        empty = new olc::Sprite(8, 8);
    }

    ~gfx() {
        for (int i=0; i<0x400; i++)
            delete data[i];
        delete empty;
    }

    //array getter (overflow safe)
    olc::Sprite* get(int idx) {
        return (0 <= idx && idx < 0x400) ? data[idx] : empty;
    }

    //array getter (overflow safe)
    olc::Sprite* operator[](int idx) {return get(idx);}


    //load a gfx from file
    int load(std::string name, int offset) {
        FILE* file = std::fopen(name.c_str(), "rb");
        std::fseek(file, 0, SEEK_END);
        int count = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);

        uint8_t* raw = (uint8_t*)malloc(count*sizeof(uint8_t));
        fread(raw, sizeof(uint8_t), count, file);

        uint8_t pixel;
        const int tileSize = 8*4;

        
        log_console << "[INFO] <> loading GFX <" << name << "> at tile offset: 0x" << std::hex << offset << std::dec << "\n";
        for(int i=0; i<count/tileSize; i++)
        {
            if (i+offset >= 0x400) {
                log_console << "[ERROR] <> loading overflowed at tile 0x" <<std::hex<< i <<std::dec<< ", loading cut short\n";
                break;
            }
            for (int y=0; y<8; y++)
            for (int x=0; x<8; x++) {
                pixel = 0;
                if (raw[tileSize*i + 2*y] & (0x80>>x)) pixel |= 0b0001;
                if (raw[tileSize*i + 2*y+1] & (0x80>>x)) pixel |= 0b0010;
                if (raw[tileSize*i + tileSize/2 + 2*y] & (0x80>>x)) pixel |= 0b0100;
                if (raw[tileSize*i + tileSize/2 + 2*y+1] & (0x80>>x)) pixel |= 0b1000;
                
                data[i+offset]->SetPixel({x,y}, olc::Pixel(pixel, 0, 0, 0xFF));
            }
        }
        free(raw);
        fclose(file);
        return count/tileSize;
    }

    //print of of the sprites to the screen
    bool printTile(window *win, int tile, olc::vi2d at) {
        if (tile >= 0x400) return false;
        win->DrawSprite(at, data[tile]);
        return true;
    }
};




/**************************************** snes::tile ****************************************/


struct snes::tile {
    int id;         // bits 0-9
    uint8_t flip;   // bits 14-15
    uint8_t palette;// bits 10-12
    uint8_t prio;   // bits 13
};


/**************************************** snes::tiles ****************************************/



//sould not be copied as it countains an malloced array
struct snes::tiles {
    int sx, sy;
    tile* data;

    //malloc the dynamic tile array
    tiles(olc::vi2d size) {
        sx = size.x;
        sy = size.y;
        data = (tile*) malloc(sx*sy*sizeof(tile));
    }

    //make non copyable
    tiles(const tiles&) = delete;
    tiles& operator=(const tiles&) = delete;

    //free
    ~tiles() {
        free(data);
    }

    //array getter
    tile& get(olc::vi2d at) {
        return data[pos2idx(sx)(at)];
    }

    //same getter
    tile& operator[](olc::vi2d at) {
        return get(at);
    }

    //flip the array and its tiles horizontally
    void fliph() {
        tile* hold = new tile[sx];
        
        for (int y=0; y<sy; y++)
        {
            for (int x=0; x<sx; x++) {
                //XOR the x axis with 1 to flip
                get({x, y}).flip ^= snes::axis::xo;

                //store in flipped order
                hold[(sx-1)-x] = get({x, y});
            }
            for (int x=0; x<sx; x++) {
                //restore the tiles in order
                get({x, y}) = hold[x];
            }
        }
        delete[] hold;
    }

    //flips the array and its tiles vertically
    void flipv() {
        tile* hold = new tile[sy];
        
        for (int x=0; x<sx; x++)
        {
            for (int y=0; y<sy; y++) {
                //XOR the y axis with 1 to flip
                get({x,y}).flip ^= snes::axis::oy;

                //store in flipped oreder
                hold[(sy-1)-y] = get({x,y});
            }
            for (int y=0; y<sy; y++) {
                //restore the tiles in order
                get({x,y}) = hold[y];
            }
        }
    }
};


/**************************************** snes::tilemap ****************************************/


struct snes::tilemap {
    static const int sx = 32;
    static const int sy = 32;
    snes::tile defTile;
    tile data[sx * sy];


    //initialize to a default value
    tilemap() {
        defTile = {0, snes::axis::oo, 0, 0};
        for (int i=0; i<(sx * sy); i++) {
            data[i] = {-1, snes::axis::oo, 0, 0};
        }
    }

    //array getter
    tile& get(int idx) {
        return data[idx];
    }

    //2d array getter
    tile& get(olc::vi2d at) {
        return data[pos2idx(sx)(at)];
    }

    //operator aliases
    tile& operator[](olc::vi2d at) {return get(at);}
    tile& operator[](int idx) {return get(idx);}


    //paste a tiles structure
    void pasteTiles(olc::vi2d at, snes::tiles* tiles) {
        for (int y=0; y<tiles->sy; y++) {
            for (int x=0; x<tiles->sx; x++) {
                olc::vi2d loc = at + olc::vi2d(x,y);
                if (loc.x < 32 && loc.y < 32) {
                    get(loc) = tiles->get({x,y});
                }
            }
        }
    }

    //paste tiles from a raw pointer to a tile array w/ memcpy
    void pasteTiles(olc::vi2d at, olc::vi2d size, tile* tiles) {
        for (int y=0; y<size.y; y++) {
            int lin_off = y * size.x;
            int new_loff = (at.y + y) * size.x + at.x;
            std::memcpy((data + new_loff), (tiles + lin_off), size.x);
        }
    }

    //query the tile numbers from this tilemap in a region
    //specified in pixel coordinates
    snes::tiles* getTiles(olc::vi2d start, olc::vi2d end) {
        olc::vi2d start_t = start / 8;
        olc::vi2d end_t = end / 8;
        olc::vi2d size = end_t - start_t;
        snes::tiles* res = new snes::tiles(size);
        
        for (int y=0; y<size.y; y++)
        for (int x=0; x<size.x; x++)
        {
            res->get({x,y}) = get({start_t.x+x, start_t.y+y});
        }
        return res;
    }
};
