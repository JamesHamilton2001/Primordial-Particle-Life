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

    auto valf = [cleanSpaces](std::istream& ss, std::string& str, float& f) {
        if (!std::getline(ss, str, ',')) return false;
        cleanSpaces(str);
        try { f = std::stof(str); }
        catch (const std::invalid_argument& e) { return false; }
        return true;
    }; auto vali = [cleanSpaces](std::istream& ss, std::string& str, int& i) {
        if (!std::getline(ss, str, ',')) return false;
        cleanSpaces(str);
        try { i = std::stoi(str); }
        catch (const std::invalid_argument& e) { return false; }
        return true;
    };
    auto linef = [cleanSpaces, cleanCommas, valf](std::ifstream& file, std::string& line, float& f) {
        if (!std::getline(file, line)) return false;
        cleanSpaces(line), cleanCommas(line);
        try { f = std::stof(line); }
        catch (const std::invalid_argument& e) { return false; }
        return true;
    }; auto linei = [cleanSpaces, cleanCommas](std::ifstream& file, std::string& line, int& i) {
        if (!std::getline(file, line)) return false;
        cleanSpaces(line), cleanCommas(line);
        try { i = std::stoi(line); }
        catch (const std::invalid_argument& e) { return false; }
        return true;
    };
    auto matf = [valf](std::ifstream& file, std::string& line, std::vector<std::vector<float>>& mat) {
        for (auto& row : mat) {
            std::getline(file, line);
            std::stringstream ss(line);
            std::string str;
            for (float& f : row) valf(ss, str, f);
        }
    };
    auto pline = [vali, valf](std::ifstream& file, std::string& line, std::vector<Particle>& particles) {
        if(!std::getline(file, line)) return false;
        std::stringstream ss(line);
        std::string str;
        Particle p;
        if (!vali(ss, str, p.type) || !valf(ss, str, p.pos.x) || !valf(ss, str, p.pos.y) ||
            (valf(ss, str, p.vel.x) && !valf(ss, str, p.vel.y)))
            return false;
        return particles.emplace_back(p), true;
    };
    
    std::string path = "settings/default/";
    for (const auto& entry : fs::directory_iterator(path)) {
        std::string filename = entry.path().filename().string();
        std::ifstream file(path + entry.path().filename().string());
        std::string line;
        ParticleLife::Settings set;

        set.name = filename.substr(0, filename.length() - 4);

        linei(file, line, set.types);
        linei(file, line, set.size);
        linei(file, line, set.count);
        linef(file, line, set.innerRadius);
        linef(file, line, set.resistance);
        linef(file, line, set.step);

        set.attractions = std::vector<std::vector<float>>(set.types, std::vector<float>(set.types, 0));
        matf(file, line, set.attractions);

        for (int i = 0; i < set.count && pline(file, line, set.particles); i++);

        linei(file, line, set.seed);

        defaultSettings.push_back(set);
        
        file.close();
    }
}

