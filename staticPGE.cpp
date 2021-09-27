#pragma once

#include "olcPixelGameEngine.h"

#include <cstring> //std::memcpy
#include <stack>


/*
functions to do olc::PixelGameEngine stuff without an instance
    -DrawSprite(src, target, pos)
*/

#define _this (*this)

namespace olc {
    //transparent black
    const olc::Pixel TRS = (0,0,0,0);

    void DrawSprite(olc::Sprite &src, olc::Sprite &target, olc::vi2d at) {
        for (int x=0; x<src.width; x++)
        for (int y=0; y<src.height; y++) {
            target.SetPixel({at.x+x, at.y+y}, src.GetPixel({x,y}));
        }
    }

    //keep track of all white click effects to destroy them after a few frames
    struct EffArray : public std::vector<std::pair<float, std::pair<olc::vi2d, olc::vi2d>>>
    {

        void newEffect(float time_ms, std::pair<olc::vi2d, olc::vi2d> area);

    };
}

