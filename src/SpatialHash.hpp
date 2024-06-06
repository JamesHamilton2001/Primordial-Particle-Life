#ifndef SPATIALHASH_HPP
#define SPATIALHASH_HPP

#include "Particle.hpp"

#include <raylib.h>
#include <vector>
#include <iostream>


class SpatialHash
{
 public:

    SpatialHash(unsigned int size, unsigned int types);

    unsigned int hash(float coord) const;                        // hash y for row, x for col

    void map(vector<Particle>& particles);         // map particles to grid & overlaps

    vector<Particle*>& getCell(unsigned int row, unsigned int col);

    const vector<vector<Particle>>& getCornerWraps() const;
    const vector<vector<vector<Particle>>>& getEdgeWraps() const;

 private:

    const unsigned int types;    // number of particle types
    const unsigned int size;     // number of cells in one dimension
    const float bounds; // in world grid bounds

    // [size+2][size+2], corner and edge wraps
    vector<vector<vector<Particle*>>> grid;
    
    // TL, TR, BR, BL
    vector<vector<Particle>> cornerWraps;

    // T[size],  R[size],  B[size],  L[size]
    vector<vector<vector<Particle>>> edgeWraps;
    
};


#endif