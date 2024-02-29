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
    auto cleanSpaces = [](std::string& s) { s.erase(std::remove_if(s.begin(), s.end(), [](char c) { return std::isspace(c); }), s.end()); };
    auto cleanCommas = [](std::string& s) { s.erase(std::remove_if(s.begin(), s.end(), [](char c) { return c == ','; }), s.end()); };

    auto dataCount = [](const std::string& s) {
        if (s.empty() || (s.size() == 1 && s[0] == ',')) return 0;
        int count = 1;
        for (char c : s) if (c == ',') count++;
        return count;
    };
    
    std::string path = "settings/default/";
    std::vector<std::string> filenames;
    for (const auto& entry : fs::directory_iterator(path))
        filenames.push_back(entry.path().filename().string());

    // load default settings
    for (const auto& filename : filenames)
    {
        std::ifstream file(path + filename);
        std::string line;
        
        defaultSettings.emplace_back(ParticleLife::Settings());
        ParticleLife::Settings& set = defaultSettings.back();

        set.name = filename.substr(0, filename.length() - 4);

        std::getline(file, line), cleanSpaces(line), cleanCommas(line); // types
        set.types = std::stoi(line);
        std::getline(file, line), cleanSpaces(line), cleanCommas(line); // size
        set.size = std::stoi(line);
        std::getline(file, line), cleanSpaces(line), cleanCommas(line); // count
        set.count = std::stoi(line);
        std::getline(file, line), cleanSpaces(line), cleanCommas(line); // innerRadius
        set.innerRadius = std::stof(line);
        std::getline(file, line), cleanSpaces(line), cleanCommas(line); // resistance
        set.resistance = std::stof(line);
        std::getline(file, line), cleanSpaces(line), cleanCommas(line); // step
        set.step = std::stof(line);

        // attractions
        for (int i = 0; i < set.types; i++) {
            set.attractions.emplace_back(std::vector<float>());
            std::getline(file, line);
            cleanSpaces(line);
            std::stringstream ss(line);
            std::string str;
            for (int j = 0; j < set.types; j++)
                std::getline(ss, str, ','),
                set.attractions[i].emplace_back(std::stof(str));
        }

        // particles
        set.particles = std::vector<Particle>(0);
        for (int i = 0; i < set.count; i++) {
            std::getline(file, line), cleanSpaces(line);
            if (dataCount(line) == 0) break;
            std::stringstream ss(line);
            std::string str;
            int t;
            float x, y;
            std::getline(ss, str, ','), cleanSpaces(str);
            t = std::stoi(str);
            std::getline(ss, str, ','), cleanSpaces(str);
            x = std::stof(str);
            std::getline(ss, str, ','), cleanSpaces(str);
            y = std::stof(str);
            float vx = 0, vy = 0;
            if (std::getline(ss, str, ',')) { cleanSpaces(str);
                vx = std::stof(str);
                std::getline(ss, str, ','), cleanSpaces(str);
                vy = std::stof(str);
            }
            set.particles.emplace_back(Particle(t, Vector2 { x, y }, Vector2 { vx, vy }));
        }

        // seed
        std::getline(file, line), cleanSpaces(line), cleanCommas(line); // seed
        set.seed = std::stoi(line);
        
        file.close();
    }
}

