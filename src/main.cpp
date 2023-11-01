#include "ParticleLife.hpp"

#include <raylib.h>

#include <iostream>



int windowWidth;
int windowHeight;
int fpsTarget;

int particleTypes;
int initialCount;
float innerRadius;
float resistance;
float step;
int gridSize;

ParticleLife particleLife;
Camera2D camera;



int main()
{
    particleTypes = 5;
    initialCount = 1024;
    innerRadius = 0.25f;
    resistance = 0.0025f;
    step = 0.000025f;
    gridSize = 24;

    windowWidth = 1600;
    windowHeight = 800;
    fpsTarget = 0;

    InitWindow(windowWidth, windowHeight, "Primordial Particle Life");
    SetTargetFPS(fpsTarget);

    particleLife.init(particleTypes, initialCount, innerRadius, resistance, step, gridSize);

    camera.offset = { windowWidth/2.0f, windowHeight/2.0f };
    camera.target = { (float)(gridSize), (float)(gridSize) };
    camera.rotation = 0.0f;
    camera.zoom = 25.0f;

    while (!WindowShouldClose())
    {
        particleLife.update();

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode2D(camera);
                particleLife.draw();
            EndMode2D();

            DrawFPS(windowWidth - 80, 5);
        EndDrawing();
    }
}
