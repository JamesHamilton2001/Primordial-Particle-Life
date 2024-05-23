#ifndef PARTICLELIFE_HPP
#define PARTICLELIFE_HPP

#include "Settings.hpp"
#include "Particle.hpp"
#include "SpatialHash.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <vector>
#include <iostream>
#include <random>
#include <string>



class ParticleLife
{
  public:

    ParticleLife() = default;
    ParticleLife(Settings& settings);
    ~ParticleLife();
    ParticleLife& operator =(const ParticleLife& other) = default;

    std::string name;
    int types;
    int size;
    float bounds;


    void update();
    void draw(unsigned int pTexID) const;

    void saveConfig() const;

    void drawGrid() const   { spatialHash.drawGrid(); }
    void drawGhosts(unsigned int pTexID) const { spatialHash.drawGhosts(pTexID); }
    void drawSoftBorder() const;

    void randomisePositions();

    std::vector<int> countTypes() const;

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

    int seed;

    SpatialHash spatialHash;

    std::mt19937 gen;
    std::uniform_real_distribution<float> posDistr;

    void particleInteraction(Particle& p1, Particle& p2);

    void debugGrid();

};


#endif