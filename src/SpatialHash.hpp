#ifndef SPATIALHASH_HPP
#define SPATIALHASH_HPP

#include "Particle.hpp"

#include <raylib.h>
#include <vector>
#include <iostream>


class SpatialHash
{
 public:

    SpatialHash(int size, int types);

    int hash(float coord) const;                        // hash y for row, x for col

    void map(vector<Particle>& particles);         // map particles to grid & overlaps

    vector<Particle*>& getCell(int row, int col);

    void drawGrid() const;
    void drawGhosts(unsigned int texID) const;

 private:

    int types;    // number of particle types
    int size;     // number of cells in one dimension
    float bounds; // in world grid bounds

    // [size+2][size+2], corner and edge wraps
    vector<vector<vector<Particle*>>> grid;
    
    // TL, TR, BR, BL
    vector<vector<Particle>> cornerWraps;

    // T[size],  R[size],  B[size],  L[size]
    vector<vector<vector<Particle>>> edgeWraps;
    
};


#endif