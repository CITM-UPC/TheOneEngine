// TheOneBuilder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "..\TheOneEngine\EngineCore.h"
//#include <iostream>
//#include <sstream>
//#include <vector>
//#include <chrono>
//#include <thread>
//#include <string>
//#include <list>
//#include <stdlib.h>

int main(int argc, char* args[])
{
    EngineCore* eCore = new EngineCore();
    double dt = 0.16;

    eCore->Awake();
    eCore->Start();
    eCore->Update(dt);
    

    return EXIT_SUCCESS;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
