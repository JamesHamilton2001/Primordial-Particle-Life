#ifndef PARTICLELIFE_HPP
#define PARTICLELIFE_HPP

#include "Particle.hpp"

#include <raylib.h>
#include <vector>


class ParticleLife
{
 public:

    ParticleLife(int types, int size, int count, float resistance, float innerRadius, float step, std::vector<std::vector<float>> attractions, int seed);

    const int types;
    const int size;
    const float bounds;

    int count;
    float resistance;
    float innerRadius;
    float step;
    std::vector<std::vector<float>> attractions;

    std::vector<Particle> particles;

    void update();

};


#endif