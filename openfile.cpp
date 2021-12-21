
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>



void getSaveFile(char dest[MAX_PATH], LPCSTR name, LPCSTR filter)
{
    char filename[MAX_PATH];

    //make sure we start in cwd
    char curdir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, curdir);

    OPENFILENAME ofn;
        ZeroMemory(&filename, sizeof(filename));
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize     = sizeof(ofn);
        ofn.hwndOwner       = NULL;
        ofn.lpstrFilter     = filter;
        ofn.lpstrFile       = filename;
        ofn.nMaxFile        = MAX_PATH;
        ofn.lpstrTitle      = name;
        ofn.lpstrInitialDir = curdir;
        ofn.Flags           = OFN_DONTADDTORECENT;

    if (GetSaveFileName((LPOPENFILENAME)&ofn))
    {
        strcpy(dest, filename);
    } else {
        *dest = '\0';
    }
}

void getOpenFile(char dest[MAX_PATH], LPCSTR name, LPCSTR filter)
{
    char filename[MAX_PATH];

    //make sure we start in cwd
    char curdir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, curdir);

    OPENFILENAME ofn;
        ZeroMemory(&filename, sizeof(filename));
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize     = sizeof( ofn );
        ofn.hwndOwner       = NULL;
        ofn.lpstrFilter     = filter;
        ofn.lpstrFile       = filename;
        ofn.nMaxFile        = MAX_PATH;
        ofn.lpstrTitle      = name;
        ofn.lpstrInitialDir = curdir;
        ofn.Flags           = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA((LPOPENFILENAME) &ofn))
    {
        strcpy(dest, filename);
    }
    else
    {
        *dest = '\0';
    }
}