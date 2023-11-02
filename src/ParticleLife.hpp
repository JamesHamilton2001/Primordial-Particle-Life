#pragma once

#include <raylib.h>
#include <rlgl.h>

#include <vector>



class ParticleLife
{

 public:

    struct Settings {
        int typeCount;
        int count;
        float innerRadius;
        float resistance;
        float step;
        float gridSize;
        std::vector<std::vector<float>> attractions;
    };

    void init(int particleTypes, int particleCount, float particleInnerRadius, float resistance, float step, int gridSize);
    void init(Settings settings);
    void update();
    void draw();

    int getTypeCount() const;
    int getCount() const;
    float getResistance() const;
    float getInnerRadius() const;
    float getStep() const;
    int getGridSize() const;

    void randomisePositions();
    void randomiseAttractions();
    void randomiseAll();

 private:

    int typeCount;
    int count;
    float resistance;
    float innerRadius;
    float step;
    float bounds;
    
    std::vector<Color> colours;
    std::vector<std::vector<float>> attractions;

    std::vector<int> types;
    std::vector<Vector2> positions;
    std::vector<Vector2> velocities;
    
    Texture2D particleTexture;

    void initColours();
    void initTexture();

};