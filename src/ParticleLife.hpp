#pragma once

#include <raylib.h>
#include <rlgl.h>

#include <vector>


class ParticleLife
{
 public:

    void init(int particleTypes, int particleCount, int gridSize);
    void update();
    void draw();

 private:

    float step;
    float resistance;
    float bounds;
    int count;
    
    std::vector<Color> colours;
    std::vector<std::vector<float>> attractions;

    std::vector<int> types;
    std::vector<Vector2> positions;
    std::vector<Vector2> velocities;
    
    Texture2D particleTexture;

};