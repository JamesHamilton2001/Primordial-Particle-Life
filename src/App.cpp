#include "App.hpp"

#include "Particle.hpp"

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

}

App::~App()
{
    
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

    // pause on PRESS_SPACE
    if (IsKeyPressed(KEY_SPACE)) {
        paused = !paused;
        if (paused) {}
            // std::cout << particleLife << std::endl;
    }

    // toggle grid on PRESS_G
    if (IsKeyPressed(KEY_G))
        drawGrid = !drawGrid;
    
    // update simulation if not paused
    if (!paused)
        particleLife.update();
}


void App::draw(ParticleLife& particleLife) const
{
    BeginDrawing();

        ClearBackground(BLACK);

        // draw simulation scene
        BeginMode2D(camera);

            particleLife.draw();

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

        DrawFPS(10, 10);

    EndDrawing();
}
