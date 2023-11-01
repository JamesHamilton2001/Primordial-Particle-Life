#include "ParticleLife.hpp"

#include <raylib.h>

#include <iostream>



int windowWidth;
int windowHeight;
int fpsTarget;

int gridSize;

int particleTypes;
int initialParticles;

ParticleLife particleLife;
Camera2D camera;



int main()
{
    particleTypes = 3;
    initialParticles = 512;
    gridSize = 8;

    windowWidth = 1600;
    windowHeight = 800;
    fpsTarget = 60;

    InitWindow(windowWidth, windowHeight, "Primordial Particle Life");
    SetTargetFPS(fpsTarget);

    particleLife.init(particleTypes, initialParticles, gridSize);

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
