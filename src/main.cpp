#include "Launcher.hpp"
#include "App.hpp"

#define RAYGUI_IMPLEMENTATION


int main()
{
    int width = 800;
    int height = 800;

    InitWindow(width, height, "Particle Life");

    Launcher launcher;

    SetTargetFPS(60);
    while (launcher.run()) {
        if (WindowShouldClose()) {
            CloseWindow();
            return EXIT_SUCCESS;
        }
    }

    App app(width, height, 0, launcher.getSettings());

    SetTargetFPS(0);
    while (!WindowShouldClose())
        app.update(),
        app.draw(),
        app.gui();

    CloseWindow();

    return EXIT_SUCCESS;
}
