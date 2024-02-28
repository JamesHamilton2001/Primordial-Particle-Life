#include "App.hpp"

#include "Particle.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include <string>


App::App(int width, int height, int fpsTarget) :
    width(width),
    height(height),
    fpsTarget(fpsTarget),
    paused(false),
    drawGrid(true),
    drawGhosts(true),
    camera { Vector2 { float(width/2), float(height/2) },
             Vector2 { 0, 0 }, 0.0f, 25.0f             }
{
    // generate particle texture (64x64 white circle on transparent background)
    Image img = GenImageColor(64, 64, BLANK);
    ImageDrawCircle(&img, 32, 32, 32, WHITE);
    particleTexture = LoadTextureFromImage(img);
    UnloadImage(img);

    loadSettings();
}

App::~App()
{
    UnloadTexture(particleTexture);
}


void App::update(ParticleLife& particleLife)
{
    // camera zoom on MOUSE_WHEEL
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        camera.target = GetScreenToWorld2D(camera.offset, camera);
        camera.zoom += (wheel * 2.0f);
        if (camera.zoom < 2.0f)
            camera.zoom = 2.0f;
    }
    // camera pan on HOLD_RIGHT_CLICK + DRAG
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        camera.target = Vector2Add(camera.target, Vector2Scale(GetMouseDelta(), -1.0f / camera.zoom));
    
    // print cell contents on PRESS_LEFT_CLICK
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        particleLife.printCellAtPos(mouseWorldPos);
    }

    // toggle pause on PRESS_SPACE
    if (IsKeyPressed(KEY_SPACE))
        paused = !paused;

    // toggle grid on PRESS_G
    if (IsKeyPressed(KEY_G))
        drawGrid = !drawGrid;

    if (IsKeyPressed(KEY_C))
        drawGhosts = !drawGhosts;
    
    // update simulation if not paused
    if (!paused)
        particleLife.update();
}


void App::draw(ParticleLife& particleLife) const
{
    BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);

            particleLife.draw(particleTexture.id);
            if (drawGrid)   particleLife.drawGrid();
            if (drawGhosts) particleLife.drawGhosts(particleTexture.id);
            particleLife.drawSoftBorder();

        EndMode2D();

        DrawFPS(10, 10);
    EndDrawing();
}


void App::loadSettings()
{
    defaultSettings.emplace_back(ParticleLife::Settings
    {
        "debug",
        5,
        24,
        3072,
        0.55f,
        0.001f,
        0.00002f,
        { {  0.02,  0.15,  0.15,  0.20, -0.10 },
          { -0.03,  0.07,  0.10, -0.05,  0.06 },
          {  0.05,  0.10, -0.02, -0.05,  0.05 },
          {  0.10, -0.05,  0.04,  0.05,  0.10 },
          { -0.05, -0.10,  0.05,  0.10, -0.05 } },
        std::vector<Vector2>(0),
        std::vector<Vector2>(0),
        1234
    });
    defaultSettings.emplace_back(ParticleLife::Settings
    {
        "big_boy",
        3,
        4,
        15,
        0.5f,
        0.001f,
        0.001f,
        { {  0.005,  0.002,  -0.002 },
          { -0.002,  0.005,   0.002 },
          {  0.002, -0.002,   0.005 } },
        std::vector<Vector2>(0),
        std::vector<Vector2>(0),
        1234
    });
}

