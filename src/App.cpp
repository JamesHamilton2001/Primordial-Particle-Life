#include "App.hpp"

#include "Particle.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
namespace fs = std::filesystem;

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


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

    // testStringConversions();

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

    // toggle ghosts on PRESS_C
    if (IsKeyPressed(KEY_C))
        drawGhosts = !drawGhosts;
    
    // save settings to temp conifg on PRESS_ENTER
    if (paused && IsKeyPressed(KEY_ENTER))
        particleLife.saveConfig();
    
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

        DrawFPS(GetScreenWidth()-100, 10);
    EndDrawing();
}


void App::gui(ParticleLife& particleLife)
{
    GuiDrawText("GUI implementation...", Rectangle { 10, 10, 100, 20 }, TEXT_ALIGN_LEFT, RAYWHITE);
}


void App::loadSettings()
{
    for (const auto& entry : fs::directory_iterator("settings/default/")) {
        try {
            defaultSettings.push_back(ParticleLife::Settings(entry));
        } catch (std::exception& e) { 
            std::cout << "Failed to load settings from " << entry.path() << ": " << e.what() << std::endl;
        }
    }
}
