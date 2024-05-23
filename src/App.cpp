#include "App.hpp"

#include "Particle.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <filesystem>



App::App(int width, int height, int fpsTarget, Settings& settings) :
    width(width),
    height(height),
    fpsTarget(fpsTarget),
    particleLife(settings),
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
}

App::~App()
{
    UnloadTexture(particleTexture);
}


void App::update()
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
    
    // save settings and run statistics on PRESS_ENTER
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_ENTER)) {
        particleLife.saveConfig();
        runStatistics();
    }
    else if (paused && IsKeyPressed(KEY_ENTER))
        runStatistics();
    
    // update simulation if not paused
    if (!paused)
        particleLife.update();
}


void App::draw() const
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


void App::gui()
{

}

void App::runStatistics() const
{
    Settings initialSettings;
    std::vector<Particle> particles;
    long long unsigned int frameCount;
    particleLife.getComparisonData(initialSettings, particles, &frameCount);

    std::cout << "Statistics:" << std::endl;
    std::cout << "| Frame Count: " << frameCount << std::endl; 
    std::cout << "| Resulting Particle Data:" << std::endl;
    for (const Particle& p : particles)
        std::cout << "| | " << p << std::endl;

    // TODO: calculate statistics

    std::ofstream file(particleLife.settings.statisticsDir + particleLife.settings.name +"("+std::to_string(frameCount)+")" + ".json");
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing statistics." << std::endl;
        return;
    }

    // TODO: write statistics to file

    file.close();
}