#ifndef PARTICLELIFE_HPP
#define PARTICLELIFE_HPP

#include "Particle.hpp"
#include "SpatialHash.hpp"

#include <raylib.h>
#include <vector>
#include <random>


class ParticleLife
{
 public:

    struct Settings
    {
        int types;
        int size;
        int count;
        float resistance;
        float innerRadius;
        float step;
        std::vector<std::vector<float>> attractions;
        int seed;
    };

    ParticleLife(Settings settings);
    ~ParticleLife();

    const int types;
    const int size;
    const float bounds;


    void update();
    void draw(unsigned int pTexID) const;

    void drawGrid() const   { spatialHash.drawGrid(); }
    void drawGhosts(unsigned int pTexID) const { spatialHash.drawGhosts(pTexID); }
    void drawSoftBorder() const;

    void randomisePositions();

    void printCell(int row, int col);
    void printCellAtPos(Vector2 pos);

    friend std::ostream& operator << (std::ostream& os, const ParticleLife& particleLife);

 private:

    int count;
    float resistance;
    float innerRadius;
    float step;
    std::vector<std::vector<float>> attractions;

    std::vector<Particle> particles;

    SpatialHash spatialHash;

    std::mt19937 gen;
    std::uniform_real_distribution<float> posDistr;

    void particleInteraction(Particle& p1, Particle& p2);

    void debugGrid();

};


#endif