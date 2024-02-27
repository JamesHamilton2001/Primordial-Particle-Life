#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <raylib.h>
#include <iostream>

#include "ParticleLife.hpp"


class App
{
 public:

    App(int width, int height, int fpsTarget);
    ~App();

    void update(ParticleLife& particleLife);
    void draw(ParticleLife& particleLife) const;
    
 private:

    unsigned int width;
    unsigned int height;
    unsigned int fpsTarget;

    bool paused;
    bool drawGrid;
    bool drawGhosts;

    Camera2D camera;
    Texture2D particleTexture;

};


#endif