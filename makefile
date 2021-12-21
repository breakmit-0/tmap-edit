libs = -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi
options = -Werror -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-parameter
src = main.cpp 
out = .build/a.exe


main:
	g++ -o $(out) $(src) $(libs) $(options) -mwindows -lstdc++fs -static -std=c++17
