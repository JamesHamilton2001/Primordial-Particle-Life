#include "ParticleLife.hpp"

#include <raylib.h>

#include <iostream>



int windowWidth;
int windowHeight;
int fpsTarget;

int particleTypes;
int initialParticles;
float innerRadius;
float resistance;
float step;
int gridSize;

ParticleLife particleLife;
Camera2D camera;



int main()
{
    particleTypes = 5;
    initialParticles = 512;
    innerRadius = 0.25f;
    resistance = 0.0025f;
    step = 0.000025f;
    gridSize = 16;

    windowWidth = 1600;
    windowHeight = 800;
    fpsTarget = 165;

    InitWindow(windowWidth, windowHeight, "Primordial Particle Life");
    SetTargetFPS(fpsTarget);

    particleLife.init(particleTypes, initialParticles, innerRadius, gridSize);

    camera.offset = { windowWidth/2.0f, windowHeight/2.0f };
    camera.target = { (float)(gridSize), (float)(gridSize) };
    camera.rotation = 0.0f;
    camera.zoom = 25.0f;

    std::cout << std::endl << "Output:" << std::endl << std::endl;


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
