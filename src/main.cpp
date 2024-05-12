#include "Launcher.hpp"
#include "App.hpp"

#include <iostream>

#define RAYGUI_IMPLEMENTATION


int main()
{
    int width = 1024;
    int height = 768;

    std::cout << std::endl;
    InitWindow(width, height, "Particle Life");
    SetTargetFPS(30);
    std::cout << std::endl;

    Launcher launcher;
    while (launcher.run()) {
        if (WindowShouldClose()) {
            CloseWindow();
            return EXIT_SUCCESS;
        }
    }

    App app(width, height, 0, launcher.getSettings());

    std::cout << std::endl;
    SetTargetFPS(0);
    std::cout << std::endl;
    
    while (!WindowShouldClose())
        app.update(),
        app.draw(),
        app.gui();

    CloseWindow();

    return EXIT_SUCCESS;
}
