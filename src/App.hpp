#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <raylib.h>
#include <iostream>
#include <vector>
#include <string>

#include "ParticleLife.hpp"


class App
{
 public:

    App(int width, int height, int fpsTarget, Settings& settings);
    ~App();

    void update();
    void draw() const;
    void gui();

 private:

    int width;
    int height;
    int fpsTarget;

    ParticleLife particleLife;

    bool paused;
    bool drawGrid;
    bool drawGhosts;
    
    Camera2D camera;
    Texture2D particleTexture;

    void runStatistics() const;

};


#endif