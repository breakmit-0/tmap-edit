p_libs = -luser32 -lgdi32 -lopengl32 -lgdiplus -lshlwapi -ldwmapi
p_options = -Werror -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-parameter
p_src = code/main.cpp .build/icon.res
p_out = .build/tmap-edit.exe

a_options = $(p_options)
a_libs = $(p_libs)
a_src = pack.cpp
a_out = pack.exe
a_pack = assets.pck

all:
	-mkdir -r .build
	$(MAKE) -C assets
	$(MAKE) make_program
	touch .build/app.log

clean:
	-rm -r .build
	mkdir .build

make_program:
	x86_64-w64-mingw32-windres assets/icon/app.rc -O coff -o .build/icon.res
	x86_64-w64-mingw32-g++ -o $(p_out) $(p_src) $(p_libs) $(p_options) -mwindows -lstdc++fs -static -std=c++17

