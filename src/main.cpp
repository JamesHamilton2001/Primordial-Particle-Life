#include "Launcher.hpp"
#include "App.hpp"

#include <iostream>

#define RAYGUI_IMPLEMENTATION

int main()
{
    int width = 900;
    int height = 900;
    // long long unsigned int finalFrame = 50000;
    long long unsigned int finalFrame = 0;

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
    } cout << endl;

    App app(width, height, 0, launcher.getSettings(), finalFrame);
    SetWindowSize(width, height);
    SetTargetFPS(0);

    cout << endl;
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
