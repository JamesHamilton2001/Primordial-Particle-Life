#include "App.hpp"


int main()
{
    int width = 1024;
    int height = 1024;
    int fpsTarget = 0;
    InitWindow(width, height, "Particle Life");
    SetTargetFPS(fpsTarget);

    App app(width, height, fpsTarget);

    ParticleLife particleLife(app.defaultSettings[2]);

    std::cout << particleLife << std::endl;

    while (!WindowShouldClose())
        app.update(particleLife),
        app.draw(particleLife),
        app.gui(particleLife);

    CloseWindow();

    return EXIT_SUCCESS;
}
