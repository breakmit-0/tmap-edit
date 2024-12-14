#define WINVER 0x0400
#define __WIN95__
#define __GNUWIN32__
#define STRICT
#define HAVE_W32API_H
#define __WXMSW__
#define __WINDOWS__



#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <iostream>
#include <conio.h>
#define addF(file) if (!pack->AddFile(file)) {std::cout << "PACK.EXE ERROR : file <" << file << "> does not exist, could not add it to the ressourcepack, continue?\n"; if (getch() == 'n') {std::cout << "pack.dat was not modified\n"; return 0;}}

int main() {
    olc::ResourcePack* pack = new olc::ResourcePack();

    addF("./config.png");       std::cout << "#";
    addF("./copy.png");         std::cout << "#";
    addF("./cut.png");          std::cout << "#";
    addF("./paste.png");        std::cout << "#";
    addF("./flip_h.png");       std::cout << "#";
    addF("./flip_v.png");       std::cout << "#";
    addF("./effect8x.png");     std::cout << "#";
    addF("./empty.png");        std::cout << "#";
    addF("./hover.png");        std::cout << "#";
    addF("./selbox.png");       std::cout << "#";
    addF("./tile0.png");        std::cout << "#";
    addF("./load0.png");        std::cout << "#";
    addF("./load1.png");        std::cout << "#";
    addF("./load2.png");        std::cout << "#";
    addF("./load3.png");        std::cout << "#";
    addF("./loadpal.png");      std::cout << "#";
    addF("./new.png");          std::cout << "#";
    addF("./open.png");         std::cout << "#";
    addF("./save.png");         std::cout << "#";
    addF("./saveas.png");       std::cout << "#";
    addF("./page_down.png");    std::cout << "#";
    addF("./page_up.png");      std::cout << "#";
    addF("./palselect.png");    std::cout << "#";
    addF("./prio_show.png");    std::cout << "#";
    addF("./prio_noshow.png");  std::cout << "#";
    addF("./prio.png");         std::cout << "#";
    addF("./prio2.png");        std::cout << "#";
    addF("./filler1.png");      std::cout << "#";
    addF("./filler2.png");      std::cout << "#\n";

    pack->SavePack("./pack.dat", "assets-v1.0");
    delete pack;
}
