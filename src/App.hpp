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

    App(int width, int height, int fpsTarget, Settings& settings, long long unsigned int finalFrame);
    ~App();

    bool update();
    void saveData() const;

 private:

    int width;
    int height;
    int fpsTarget;
    long long unsigned int finalFrame;
    int progressPercent;

    ParticleLife particleLife;

    bool paused;
    bool drawGrid;
    bool drawGhosts;
    
    Camera2D camera;
    Texture2D particleTexture;

    void draw() const;
    void userInput();

};


#endif