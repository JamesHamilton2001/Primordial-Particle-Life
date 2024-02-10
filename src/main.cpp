#include "App.hpp"

int main()
{
    int width = 800;
    int height = 800;
    int fpsTarget = 0;

    InitWindow(width, height, "Particle Life");
    SetTargetFPS(fpsTarget);

    App app(width, height, fpsTarget);

    ParticleLife particleLife(
        3,                              // types
        16,                             // size
        512,                            // count
        0.0025f,                        // resistance
        0.5f,                           // innerRadius
        0.0002f,                        // step
        { {  0.02f, -0.01f,  0.04f },
          { -0.02f,  0.01f, -0.02f },   // attractions
          { -0.01f,  0.03f,  0.02f } },
        1234                            // seed
    );

    while (!WindowShouldClose())
    {
        app.update(particleLife);
        app.draw(particleLife);
    }

    CloseWindow();

    return 0;

}