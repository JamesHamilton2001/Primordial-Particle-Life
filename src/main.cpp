#include "ParticleLife.hpp"

#include <raylib.h>
#include <raymath.h>

#include <iostream>



int windowWidth;
int windowHeight;
int fpsTarget;

bool paused;
bool drawGrid;

ParticleLife particleLife;
Camera2D camera;

ParticleLife::Settings redDeathSmall;
ParticleLife::Settings redDeathLarge;
ParticleLife::Settings stinkyRed;
ParticleLife::Settings debugga;


void init();
void update();
void render();

void initPreSettings();




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

    windowWidth = 600;
    windowHeight = 600;
    fpsTarget = 0;

    paused = false;
    drawGrid = false;

    InitWindow(windowWidth, windowHeight, "Primordial Particle Life");
    SetTargetFPS(fpsTarget);

    initPreSettings();

    particleLife.init(redDeathLarge);

    camera.offset = { windowWidth/2.0f, windowHeight/2.0f };
    camera.target = { (float)(particleLife.getGridSize()), (float)(particleLife.getGridSize()) };
    camera.rotation = 0.0f;
    camera.zoom = 25.0f;

}

void update()
{
    // camera pan RIGHT CLICK DRAG
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        camera.target = Vector2Add(camera.target, Vector2Scale(GetMouseDelta(), -1.0f / camera.zoom));
    
    // camera zoom on SCROLL
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        camera.target = GetScreenToWorld2D(camera.offset, camera);
        camera.zoom += (wheel * 2.0f);
        if (camera.zoom < 2.0f)
            camera.zoom = 2.0f;
    }

    // toggle pause on press SPACE
    if (IsKeyPressed(KEY_SPACE))
        paused = !paused;

    // toggle drawn grid on press G
    if (IsKeyPressed(KEY_G))
        drawGrid = !drawGrid;

    // randomise on press R
    if (IsKeyPressed(KEY_R))
        particleLife.randomiseAll();
    
    // update simulation
    if (!paused)
        particleLife.update();

}

void render()
{
    BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);

            // draw simulation
            particleLife.draw();

            // draw grid lines if toggled on
            if (drawGrid) {
                int gridSize = particleLife.getGridSize();
                rlBegin(RL_LINES);
                    rlColor4ub(80, 80, 80, 255);
                    for (int i = 0; i <= gridSize; i++)
                        rlVertex2i(0, i*2), rlVertex2i(gridSize*2, i*2),
                        rlVertex2i(i*2, 0), rlVertex2i(i*2, gridSize*2);
                rlEnd();
            }

        EndMode2D();

        DrawFPS(windowWidth - 80, 5);
        
    EndDrawing();
}


void initPreSettings()
{
    redDeathLarge.typeCount   = 5;
    redDeathLarge.count       = 4000;
    redDeathLarge.innerRadius = 0.5f;
    redDeathLarge.resistance  = 0.0025f;
    redDeathLarge.step        = 0.0002f;
    redDeathLarge.gridSize    = 48;
    redDeathLarge.attractions.resize(redDeathLarge.typeCount, std::vector<float>(redDeathLarge.typeCount, 0.0f));
    redDeathLarge.attractions[0][0] = 0.8f;
    for (int i = 1; i < redDeathLarge.typeCount; i++)
        redDeathLarge.attractions[0][i] = 0.6f;
    for (int i = 1; i < redDeathLarge.typeCount; i++)
        redDeathLarge.attractions[i][0] = -0.6f;
    for (int i = 1; i < redDeathLarge.typeCount; i++)
        for (int j = 0; j < redDeathLarge.typeCount; j++)
            redDeathLarge.attractions[i][j] = -0.05f + (i+j)%4 * 0.05f;


    redDeathSmall = redDeathLarge;
    redDeathSmall.count = 512;
    redDeathSmall.gridSize = 16;


    stinkyRed.typeCount   = 3;
    stinkyRed.count       = 2000;
    stinkyRed.innerRadius = 0.5f;
    stinkyRed.resistance  = 0.0025f;
    stinkyRed.step        = 0.0002f;
    stinkyRed.gridSize    = 20;
    stinkyRed.attractions.resize(stinkyRed.typeCount, std::vector<float>(stinkyRed.typeCount, 0.0f));
    stinkyRed.attractions[0][0] =  0.005f;
    stinkyRed.attractions[0][1] =  0.100f;
    stinkyRed.attractions[0][2] =  0.100f;
    stinkyRed.attractions[1][0] = -0.100f;
    stinkyRed.attractions[1][1] =  0.025f;
    stinkyRed.attractions[1][2] =  0.025f;
    stinkyRed.attractions[2][0] = -0.100f;
    stinkyRed.attractions[2][1] =  0.033f;
    stinkyRed.attractions[2][2] =  0.033f;

    debugga.typeCount   = 3;
    debugga.count       = 16;
    debugga.innerRadius = 0.5f;
    debugga.resistance  = 0.0025f;
    debugga.step        = 0.0002f;
    debugga.gridSize    = 3;
    debugga.attractions.resize(debugga.typeCount, std::vector<float>(debugga.typeCount, 0.0f));
    debugga.attractions[0][0] =  0.005f;
    debugga.attractions[0][1] =  0.100f;
    debugga.attractions[0][2] =  0.100f;
    debugga.attractions[1][0] = -0.100f;
    debugga.attractions[1][1] =  0.025f;
    debugga.attractions[1][2] =  0.025f;
    debugga.attractions[2][0] = -0.100f;
    debugga.attractions[2][1] =  0.033f;
    debugga.attractions[2][2] =  0.033f;

}