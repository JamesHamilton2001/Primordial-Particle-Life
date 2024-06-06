#include "Launcher.hpp"
#include "App.hpp"

#include <iostream>

#define RAYGUI_IMPLEMENTATION


int main()
{
    int width = 500;
    int height = 500;
    long long unsigned int finalFrame = 125000;

    cout << endl;
    InitWindow(width, height, "Particle Life");
    SetTargetFPS(30);
    cout << endl;

    Launcher launcher;
    while (launcher.run()) {
        if (WindowShouldClose()) {
            CloseWindow();
            return EXIT_SUCCESS;
        }
    }

    App app(width, height, 0, launcher.getSettings(), finalFrame);

    cout << endl;
    SetWindowSize(width, height);
    SetTargetFPS(0);
    cout << "INITIATION SETTINGS:" << endl;
    cout << launcher.getSettings() << endl;
    cout << endl;
    
    while (app.update()) {
        if (WindowShouldClose()) {
            CloseWindow();
            return EXIT_SUCCESS;
        }
    }

    CloseWindow();

    return EXIT_SUCCESS;
}
