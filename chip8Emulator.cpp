#include <iostream>
#include <vector>
#include <iterator>
#include <fstream>
#include <windows.h>
#include <cmath>
#include "CPU.h"

#define FILE_PATH "pong.rom"

// todo:
// file picker
// refactor
// add docs
// rewire test.h to cpp file
// sound
// rewite olcConsoleGameEngine
// debuger

int main()
{

    CPU a(FILE_PATH, 60);

    while (true)
    {
        a.fetch();
        Sleep(1000 / a.fpsLimit);
    }
}
