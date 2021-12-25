p_libs = -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi
p_options = -Werror -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-parameter
p_src = code/main.cpp
p_out = .build/tmap-edit.exe

a_options = $(p_options)
a_libs = $(p_libs)
a_src = pack.cpp
a_out = pack.exe
a_pack = assets.pck

all:
	rm -r ./.build/*
	$(MAKE) make_assets
	$(MAKE) make_program
	touch .build/app.log

make_program:
	g++ -o $(p_out) $(p_src) $(p_libs) $(p_options) -mwindows -lstdc++fs -static -std=c++17

make_assets:
	cd ./assets; \
		g++ -o $(a_out) $(a_src) $(a_libs) $(a_options) -lstdc++fs -static -std=c++17; \
		./$(a_out); \
		mv ./pack.dat ./$(a_pack); \
		rm ./$(a_out)
	mv ./assets/$(a_pack) ./.build/$(a_pack)
	cp ./assets/default.pal ./.build/default.pal
	cp ./assets/icon.ico ./.build/icon.ico