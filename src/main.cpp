#include "Launcher.hpp"
#include "App.hpp"

#define RAYGUI_IMPLEMENTATION


int main()
{
    int width = 800;
    int height = 800;
    int fpsTarget = 0;
    InitWindow(width, height, "Particle Life");
    SetTargetFPS(fpsTarget);

    Launcher launcher;

    while (launcher.run()) {
        if (WindowShouldClose()) {
            CloseWindow();
            return EXIT_SUCCESS;
        }
    }

    App app(width, height, fpsTarget, launcher.getSettings());

    while (!WindowShouldClose())
        app.update(),
        app.draw(),
        app.gui();

    CloseWindow();

    return EXIT_SUCCESS;
}
