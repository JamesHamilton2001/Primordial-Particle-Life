#include "ParticleLife.hpp"

#include <raylib.h>
#include <raymath.h>

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

bool drawGrid;

ParticleLife particleLife;
Camera2D camera;


void init();
void update();
void render();



int main()
{
    init();

    while (!WindowShouldClose())
        update(),
        render();
    
    CloseWindow();
    return EXIT_SUCCESS;
}


void init()
{
    particleTypes = 4;
    initialCount = 3000;
    innerRadius = 0.25f;
    resistance = 0.0025f;
    step = 0.00005f;
    gridSize = 32;

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
}

void update()
{
    // camera pan
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        camera.target = Vector2Add(camera.target, Vector2Scale(GetMouseDelta(), -1.0f / camera.zoom));
    
    // camera zoom
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        camera.target = GetScreenToWorld2D(camera.offset, camera);
        camera.zoom += (wheel * 2.0f);
        if (camera.zoom < 2.0f)
            camera.zoom = 2.0f;
    }

    // randomise on press R
    if (IsKeyPressed(KEY_R))
        particleLife.randomise();

    if (IsKeyPressed(KEY_G))
        drawGrid = !drawGrid;
    
    // update simulation
    particleLife.update();

}

void render()
{
    BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);
            particleLife.draw();

            if (drawGrid) {
                rlBegin(RL_LINES);
                    rlColor4ub(80, 80, 80, 255);
                    for (int i = 0; i <= gridSize; i++)
                        rlVertex2i(0, i*2), rlVertex2i(gridSize*2, i*2),
                        rlVertex2i(i*2, 0), rlVertex2i(i*2, gridSize*2);
            }

        EndMode2D();

        DrawFPS(windowWidth - 80, 5);
        
    EndDrawing();
}