#include "App.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>


App::App(int width, int height, int fpsTarget) :
    width(width),
    height(height),
    fpsTarget(fpsTarget)
{
    paused = false;
    drawGrid = true;

    // camera.target = Vector2 { float (particleLife.getSize()), float (particleLife.getSize()) };
    camera.target = Vector2 { 0, 0 };
    camera.offset = Vector2 { width / 2.0f, height / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 25.0f;

    Image temp = GenImageColor(64, 64, BLANK);
    ImageDrawCircle(&temp, 32, 32, 32, Color { 255, 255, 255, 255 });
    particleTexture = LoadTextureFromImage(temp);
    UnloadImage(temp);

}

App::~App()
{
    UnloadTexture(particleTexture);
}


void App::update(ParticleLife& particleLife)
{
    // camera zoom and pan
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        camera.target = GetScreenToWorld2D(camera.offset, camera);
        camera.zoom += (wheel * 2.0f);
        if (camera.zoom < 2.0f)
            camera.zoom = 2.0f;
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        camera.target = Vector2Add(camera.target, Vector2Scale(GetMouseDelta(), -1.0f / camera.zoom));
    
    if (IsKeyPressed(KEY_SPACE))    paused = !paused;
    if (IsKeyPressed(KEY_G))        drawGrid = !drawGrid;

    if (!paused)
        particleLife.update();
}


void App::draw(ParticleLife& particleLife)
{
    BeginDrawing();

        ClearBackground(BLACK);

        // draw simulation scene
        BeginMode2D(camera);

            // draw particles
            rlSetTexture(particleTexture.id);
            rlBegin(RL_QUADS);

                for (ParticleLife::Particle const& p : particleLife.particles) {
                    Color colour = particleColors[p.type];
                    rlColor4ub(colour.r, colour.g, colour.b, colour.a);
                    rlNormal3f(0.0f, 0.0f, 1.0f);

                    rlTexCoord2f(0.0f, 0.0f); rlVertex2f(p.pos.x-0.05f, p.pos.y-0.05f);
                    rlTexCoord2f(0.0f, 1.0f); rlVertex2f(p.pos.x-0.05f, p.pos.y+0.05f);
                    rlTexCoord2f(1.0f, 1.0f); rlVertex2f(p.pos.x+0.05f, p.pos.y+0.05f);
                    rlTexCoord2f(1.0f, 0.0f); rlVertex2f(p.pos.x+0.05f, p.pos.y-0.05f);
                }

            rlSetTexture(0);
            rlEnd();

            // draw grid lines if on
            if (drawGrid) {
                rlBegin(RL_LINES);
                    rlColor4ub(80, 80, 80, 255);
                    for (int i = 0; i <= particleLife.size; i++)
                        rlVertex2i(0, i*2), rlVertex2i(particleLife.size*2, i*2),
                        rlVertex2i(i*2, 0), rlVertex2i(i*2, particleLife.size*2);
                rlEnd();
            }

        EndMode2D();

    EndDrawing();
}