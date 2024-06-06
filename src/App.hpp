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

    App(int width, int height, int fpsTarget, const Settings& settings, long long unsigned int finalFrame);
    ~App();

    bool update();

 private:

    unsigned int width;
    unsigned int height;
    unsigned int fpsTarget;
    const long long unsigned int finalFrame;
    unsigned int progressPercent;

    ParticleLife particleLife;

    bool paused;
    bool drawGrid;
    bool drawGhosts;
    
    Camera2D camera;
    Texture2D particleTexture;

    void handleInput();
    void render() const;
    void saveData() const;

};


#endif