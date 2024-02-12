#ifndef PARTICLELIFE_HPP
#define PARTICLELIFE_HPP

#include "Particle.hpp"
#include "SpatialHash.hpp"

#include <raylib.h>
#include <vector>


class ParticleLife
{
 public:

    ParticleLife(int types, int size, int count, float resistance, float innerRadius, float step, std::vector<std::vector<float>> attractions, int seed);
    ~ParticleLife();

    const int types;
    const int size;
    const float bounds;

    void update();
    void draw() const;

    friend std::ostream& operator << (std::ostream& os, const ParticleLife& particleLife);

 private:

    int count;
    float resistance;
    float innerRadius;
    float step;
    std::vector<std::vector<float>> attractions;

    std::vector<Particle> particles;

    SpatialHash spatialHash;

    Texture2D particleTexture;
    std::vector<Color> particleColors = { RED, BLUE, YELLOW, PURPLE, GREEN, ORANGE, PINK, RAYWHITE, LIGHTGRAY };

    void particleInteraction(Particle& p1, Particle& p2);

};


#endif