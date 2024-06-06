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

    ParticleLife(const Settings& settings);

    const Settings settings;

    void update();
    void draw(unsigned int pTexID) const;

    long long unsigned int getFrameCount() const;
    const vector<Particle>& getParticles() const;
    // const vector<vector<Particle>*> getWrapCellPtrs() const;

    void save() const;

    void drawGrid() const;
    void drawGhosts(unsigned int pTexID) const;
    void drawSoftBorder() const;

    vector<int> countTypes() const;

    friend ostream& operator <<(ostream& os, const ParticleLife& particleLife);

  private:

    long long unsigned int frameCount;
    
    const unsigned int types;
    const unsigned int size;
    const unsigned int count;
    const float innerRadius;
    const float resistance;
    const float step;
    const vector<vector<float>> attractions;
    const float bounds;

    vector<Particle> particles;

    SpatialHash spatialHash;

    void particleInteraction(Particle& p1, Particle& p2) const;

};


#endif