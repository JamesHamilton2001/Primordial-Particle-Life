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



App::App(int width, int height, int fpsTarget, const Settings& settings, long long unsigned int finalFrame) :
    width (width),
    height (height),
    fpsTarget (fpsTarget),
    finalFrame(finalFrame),
    progressPercent (0),
    ssCount(0),
    particleLife (settings),
    paused (false),
    gridOn (false),
    ghostsOn (true),
    camera { Vector2 { float(width/2), float(height/2) },
             Vector2 { float(settings.size), float(settings.size) }, 0.0f, 16.0f }
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

bool App::update()
{
    // handle user input and update gui
    handleInput();

    // update simulation if not paused
    if (!paused) particleLife.update();

    // render simulation
    render();

    if (finalFrame != 0) {
        unsigned int frame = particleLife.getFrameCount()-1;
        unsigned int pp = int(100 * frame / finalFrame);

        // if ((pp != progressPercent && pp % 2 == 0) || frame == 1) {
        //     string ssName = "Screenshot_"+to_string(++ssCount)+"___Frame_"+to_string(frame)+".png";
        //     TakeScreenshot(ssName.c_str());
        // }

        if (pp != progressPercent){
            cout << "prog: " << pp << "%, fps: " << GetFPS() << endl;
            progressPercent = pp;
        }

        if (finalFrame > 0){
            if (particleLife.getFrameCount() >= finalFrame) {
                saveData();
                return false;
            }
        }
    }

    return true;
}

void App::saveData() const
{
    particleLife.save();

    string fileName = particleLife.settings.name +"(f"+ to_string(particleLife.getFrameCount()) +")";

    // TAKE SCREENSHOT
    TakeScreenshot((PARTICLELIFE_STATS_SCREENSHOTS_DIR + fileName + ".png").c_str());

    long long unsigned int frameCount = particleLife.getFrameCount();
    const vector<Particle>& particles = particleLife.getParticles();
    const int T = particleLife.settings.types;

    // WRITE INFORMATION TO FILE

    string filePath = PARTICLELIFE_STATS_DATA_DIR + fileName + ".json";
    ofstream file(filePath);
    if (!file.is_open()) {
        cerr << "Failed to create/open \""+filePath+"\" for writing statistics." << endl;
        return;
    }
    const Settings& s = particleLife.settings;
    const string m = "    ";

    file << "{" << endl;
    file << "\"simulation\":" << endl;
    file << "{" << endl;

    // frame count and launch settings
    file <<m<< "\"frameCount\": " << frameCount << "," << endl;
    file <<m<< "\"launchSettings\":" << endl;
    file <<m<< "{" << endl;
    file <<m+m<< "\"name\": \"" << s.name << "\"," << endl;
    file <<m+m<< "\"types\": " << s.types << "," << endl;
    file <<m+m<< "\"size\": " << s.size << "," << endl;
    file <<m+m<< "\"count\": " << s.count << "," << endl;
    file <<m+m<< "\"innerRadius\": " << s.innerRadius << "," << endl;
    file <<m+m<< "\"resistance\": " << s.resistance << "," << endl;
    file <<m+m<< "\"step\": " << s.step << "," << endl;
    file <<m+m<< "\"attractions\": [" << endl;
    for (int i = 0; i < T; i++) {
        file <<m+m+m<< '[';
        for (int j = 0; j < T; j++) {
            file << s.attractions[i][j];
            if (j < T-1) file << ", ";
        } file << ']';
        if (i < T-1) file << ',';
        file << endl;
    } file <<m+m<< "]," << endl;
    file <<m+m<< "\"seed\": " << s.seed << "," << endl;
    file <<m+m<< "\"typeRatio\": [ ";
    for (int i = 0; i < T; i++) {
        file << s.typeRatio[i];
        if (i < T-1) file << ", ";
    } file << " ]," << endl;
    file <<m+m<< "\"particles\": [" << endl <<m+m+m;
    for (int i = 0; i < s.count; i++) {
        const Particle& p = s.particles[i];
        file << "{" << endl;
        file <<m+m+m+m<< "\"t\": " << p.type << "," << endl;
        file <<m+m+m+m<< "\"x\": " << p.pos.x << "," << endl;
        file <<m+m+m+m<< "\"y\": " << p.pos.y << "," << endl;
        file <<m+m+m+m<< "\"vx\": " << p.vel.x << "," << endl;
        file <<m+m+m+m<< "\"vy\": " << p.vel.y << endl;
        file <<m+m+m<< "}";
        if (i < s.count-1) file << ',';
    } file << endl <<m+m<< "]" << endl;
    file <<m<< "}," << endl;

    // resulting particle data
    file <<m<< "\"particles\": [" << endl <<m+m;
    for (int i = 0; i < s.count; i++) {
        const Particle& p = particles[i];
        file << "{" << endl;
        file <<m+m+m<< "\"t\": " << p.type << "," << endl;
        file <<m+m+m<< "\"x\": " << p.pos.x << "," << endl;
        file <<m+m+m<< "\"y\": " << p.pos.y << "," << endl;
        file <<m+m+m<< "\"vx\": " << p.vel.x << "," << endl;
        file <<m+m+m<< "\"vy\": " << p.vel.y << endl;
        file <<m+m<< "}";
        if (i < s.count-1) file << ',';
    } file << endl <<m<< "]" << endl;

    file << "}" << endl;
    file << "}" << endl;
    file.close();
}

void App::render() const
{
    BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
            particleLife.draw(particleTexture.id);
            if (gridOn)   particleLife.drawGrid();
            if (ghostsOn) particleLife.drawGhosts(particleTexture.id);
            particleLife.drawSoftBorder();
        EndMode2D();
        DrawFPS(GetScreenWidth()-100, 10);
    EndDrawing();
}

void App::handleInput()
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
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        camera.target = Vector2Add(camera.target, Vector2Scale(GetMouseDelta(), -1.0f / camera.zoom));

    // toggle pause on PRESS_SPACE
    if (IsKeyPressed(KEY_SPACE))
        paused = !paused;

    // toggle grid on PRESS_G
    if (IsKeyPressed(KEY_G))
        gridOn = !gridOn;

    // take screenshot on PRESS_S
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
        TakeScreenshot((particleLife.settings.name +"(f"+ to_string(particleLife.getFrameCount()) + ").png").c_str());

    // save current state to settings on CTRL + S
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
        particleLife.save();

    if (IsKeyPressed(KEY_C))
        ghostsOn = !ghostsOn;

    if (IsKeyPressed(KEY_F))
        ToggleFullscreen();
}
