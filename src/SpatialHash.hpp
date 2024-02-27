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

    void map(std::vector<Particle>& particles);         // map particles to grid & overlaps

    std::vector<Particle*>& getCell(int row, int col);
    std::vector<int> countTypesInCell(int row, int col) const;

    void drawGrid() const;
    void drawGhosts(unsigned int texID) const;

 private:

    const int types;    // number of particle types
    const int size;     // number of cells in one dimension
    const float bounds; // in world grid bounds

    // [size+2][size+2], corner and edge wraps
    std::vector<std::vector<std::vector<Particle*>>> grid;
    
    // TL, TR, BR, BL
    std::vector<std::vector<Particle>> cornerWraps;

    // T[size],  R[size],  B[size],  L[size]
    std::vector<std::vector<std::vector<Particle>>> edgeWraps;
    
};


#endif