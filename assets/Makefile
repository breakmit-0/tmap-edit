
a_options = $(p_options)
p_libs = -luser32 -lgdi32 -lopengl32 -lgdiplus -ldwmapi -lshlwapi
a_libs = $(p_libs) # -lstdc++ -lgcc -lodbc32 -lwsock32 -lwinspool -lwinmm -lshell32 -lcomctl32 -lctl3d32 -lodbc32 -ladvapi32 -lodbc32 -lwsock32 
a_src = pack.cpp
a_pack = assets.pck

.PHONY: make_assets

make_assets: ../.build/pack.exe
	../.build/pack.exe
	mv ./pack.dat ./$(a_pack)
	mv $(a_pack) ../.build/$(a_pack)
	cp default.pal ../.build/default.pal
	cp icon/app.ico ../.build/icon.ico

../.build/pack.exe: ./pack.cpp
	x86_64-w64-mingw32-g++ -o ../.build/pack.exe $(a_src) $(a_libs) $(a_options) -lstdc++fs --static -std=c++17


