#ifndef PARTICLELIFE_HPP
#define PARTICLELIFE_HPP

#include "Settings.hpp"
#include "Particle.hpp"
#include "SpatialHash.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <vector>
#include <iostream>
#include <string>



class ParticleLife
{
  public:

    ParticleLife(Settings& settings);

    const Settings settings;

    void update();
    void draw(unsigned int pTexID) const;

    void saveConfig() const;

    void drawGrid() const;
    void drawGhosts(unsigned int pTexID) const;
    void drawSoftBorder() const;

    void randomisePositions();

    vector<int> countTypes() const;

    void printCell(int row, int col);
    void printCellAtPos(Vector2 pos);

    void getComparisonData(Settings& settings, vector<Particle>& particles, vector<Particle> ghosts, long long unsigned int* frameCount) const;

    friend ostream& operator << (ostream& os, const ParticleLife& particleLife);

  private:

    long long unsigned int frameCount;

    const int types;
    const int size;
    const float bounds;
    const int count;
    const float resistance;
    const float innerRadius;
    const float step;
    const vector<vector<float>> attractions;

    vector<Particle> particles;

    SpatialHash spatialHash;

    void particleInteraction(Particle& p1, Particle& p2);

};


#endif