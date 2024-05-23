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
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        camera.target = Vector2Add(camera.target, Vector2Scale(GetMouseDelta(), -1.0f / camera.zoom));

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
        // particleLife.saveConfig(); // TODO: re-enable saving
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
    Settings settings;
    vector<Particle> particles;
    vector<Particle> ghosts;
    long long unsigned int frameCount;
    particleLife.getComparisonData(settings, particles, ghosts, &frameCount);

    const int T = settings.types;

    // PARTICLE SPEEDS BY TYPE

    auto typeSpeeds = vector<vector<float>>(T);
    auto totalTypeSpeeds = vector<float>(T, 0);
    auto meanTypeSpeeds = vector<float>(T);

    for (const Particle& p : particles) {
        float speed = settings.step * Vector2Length(p.vel);
        typeSpeeds[p.type].emplace_back(speed);
        totalTypeSpeeds[p.type] += speed;
    } for (int i = 0; i < T; i++)
        meanTypeSpeeds[i] = totalTypeSpeeds[i] / typeSpeeds[i].size();

    // PARTICLE INTERACTIONS BY TYPE

    auto t2tInterDists = vector<vector<vector<float>>>(T, vector<vector<float>>(T));
    auto t2tInterDistSums = vector<vector<float>>(T, vector<float>(T, 0.0f));
    auto t2tInterDistMeans = vector<vector<float>>(T, vector<float>(T, 0.0f));

    for (const Particle& p1 : particles) {
        for (const Particle& p2 : particles) {
            if (&p1 == &p2) continue;
            float dist = Vector2Distance(p1.pos, p2.pos);
            if (dist <= 2.0f)
                t2tInterDists[p1.type][p2.type].emplace_back(Vector2Distance(p1.pos, p2.pos));
        }
        for (const Particle& g : ghosts) {
            float dist = Vector2Distance(p1.pos, g.pos);
            if (dist <= 2.0f)
                t2tInterDists[p1.type][g.type].emplace_back(Vector2Distance(p1.pos, g.pos));
        }
    } for (int i = 0; i < T; i++) {
        for (int j = 0; j < T; j++) {
            for (float dist : t2tInterDists[i][j])
                t2tInterDistSums[i][j] += dist;
            t2tInterDistMeans[i][j] = t2tInterDistSums[i][j] / t2tInterDists[i][j].size();
        }
    }

    // PRINT STATISTICS

    cout << "Statistics: Frame: " << frameCount << endl; 

    cout << "| Mean Type Speeds: " << endl;
    for (int i = 0; i < T; i++)
        cout << "| | ["<<i<<"]: " << meanTypeSpeeds[i] << endl;

    cout << "| Type Interactions: (count, mean)" << endl;
    for (int i = 0; i < T; i++)
        for (int j = 0; j < T; j++)
            cout <<"| | ["<<i<<"]["<<j<<"]: " << t2tInterDists[i][j].size() <<", "<< t2tInterDistMeans[i][j] << endl;

    // WRITE STATISTICS TO FILE

    ofstream file(settings.statisticsDir + settings.name +"("+to_string(frameCount)+")" + ".json");
    if (!file.is_open()) {
        cerr << "Failed to open file for writing statistics." << endl;
        return;
    }
    const string m = "    ";
    file << "{" << endl;

    // frame count and launch settings
    file <<m<< "\"frameCount\": " << frameCount << "," << endl;
    file <<m<< "\"launchSettings\":" << endl;
    file <<m<< "{" << endl;
    file <<m+m<< "\"name\": \"" << settings.name << "\"," << endl;
    file <<m+m<< "\"types\": " << settings.types << "," << endl;
    file <<m+m<< "\"size\": " << settings.size << "," << endl;
    file <<m+m<< "\"count\": " << settings.count << "," << endl;
    file <<m+m<< "\"innerRadius\": " << settings.innerRadius << "," << endl;
    file <<m+m<< "\"resistance\": " << settings.resistance << "," << endl;
    file <<m+m<< "\"step\": " << settings.step << "," << endl;
    file <<m+m<< "\"attractions\": [" << endl;
    for (int i = 0; i < T; i++) {
        file <<m+m+m<< '[';
        for (int j = 0; j < T; j++) {
            file << settings.attractions[i][j];
            if (j < T-1) file << ", ";
        } file << ']';
        if (i < T-1) file << ',';
        file << endl;
    } file <<m+m<< "]," << endl;
    file <<m+m<< "\"seed\": " << settings.seed << "," << endl;
    file <<m+m<< "\"typeRatio\": [ ";
    for (int i = 0; i < T; i++) {
        file << settings.typeRatio[i];
        if (i < T-1) file << ", ";
    } file << " ]," << endl;
    file <<m+m<< "\"particles\": [" << endl;
    for (int i = 0; i < settings.count; i++) {
        const Particle& p = settings.particles[i];
        file <<m+m+m<< "{\n";
        file <<m+m+m+m<< "\"type\": " << p.type << "," << endl;
        file <<m+m+m+m<< "\"xPos\": " << p.pos.x << "," << endl;
        file <<m+m+m+m<< "\"yPos\": " << p.pos.y << "," << endl;
        file <<m+m+m+m<< "\"xVel\": " << p.vel.x << "," << endl;
        file <<m+m+m+m<< "\"yVel\": " << p.vel.y << endl;
        file <<m+m+m<< "}";
        if (i < settings.count-1) file << ',';
        file << endl;
    } file <<m+m<< "]" << endl;
    file <<m<< "}," << endl;

    file << "}" << endl;
    file.close();
}