
namespace butt {
    namespace main {
        enum name { 
            newfile = 0,
            open = 1,
            save = 2,
            saveas = 3,
            copy = 4,
            cut = 5,
            paste = 6,
            flipv = 7,
            fliph = 8,
            prio = 9,
            prio2 = 10,
            config = 11,
            pal = 12,
            empty = -1
        };
    };
    namespace alt {
        enum name {
            //actables
            gfx0 = 0,
            gfx1 = 1,
            gfx2 = 2,
            gfx3 = 3,
            page = 4,
            empty = 5,
            tile0 = 6,
            showprio = 7
        };
    };
    namespace sprite {
        enum name{
            open,
            newfile,
            save,
            saveas,
            copy,
            cut,
            paste,
            flipv,
            fliph,
            prio,
            prio2,
            gfx0,
            gfx1,
            gfx2,
            gfx3,
            page_up,
            page_down,
            pal,
            fill1,
            fill2,
            empty,
            palmap,
            tile0,
            config,
            prio_show,
            prio_noshow
        };
    };
};
namespace sprite {
    enum name{
        selbox,
        hover,
        effect8x
    };
};
namespace mode {
    enum name {
        none,
        map,
        bank
    };
};
namespace phase {
    enum name {
        none,
        map,
        bank
    };
};
namespace snes {
    namespace axis {
        enum {
            oo = 0b00,
            xo = 0b01,
            oy = 0b10,
            xy = 0b11
        };
    };
};