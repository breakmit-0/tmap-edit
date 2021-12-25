#include "../olcPixelGameEngine.h"


/*
generic util functions
*/

//test if point is in box
template<class T>
bool is_in(olc::v2d_generic<T> pos, T box[4]) {
    return 
        pos.x >= box[0] &&
        pos.y >= box[1] &&
        pos.x <  box[2] &&
        pos.y <  box[3];
}

//same but with a const box
template<class T>
bool is_in(olc::v2d_generic<T> pos, const T box[4]) {
    return 
        pos.x >= box[0] &&
        pos.y >= box[1] &&
        pos.x <  box[2] &&
        pos.y <  box[3];
}

//convert 1d index to 2d
auto idx2pos(int width) {
    return [width] (int idx) -> olc::vi2d {
        return {idx % width, idx / width};
    };
}

//convert 2d index to 1d
auto pos2idx(int width) {
    return [width] (olc::vi2d pos) -> int {
        return pos.y * width + pos.x;
    };
}

//effectively min extended to vectors
olc::vi2d clampUp(olc::vi2d target, olc::vi2d limit) {
    return {
        std::min(target.x, limit.x),
        std::min(target.y, limit.y)
    };
}
