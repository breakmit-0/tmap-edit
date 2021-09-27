libs = -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi
src = editor.cpp
out = a.exe


main:
	g++ -o $(out) $(src) $(libs)  -lstdc++fs -static -std=c++17 -Werror