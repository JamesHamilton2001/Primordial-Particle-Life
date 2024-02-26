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

    int hash(float coord) const;
    void map(std::vector<Particle>& particles);

    std::vector<Particle*>& getCell(int row, int col);
    std::vector<Particle>& getCornerWrap(int corner);
    std::vector<Particle>& getEdgeWrap(int edge);

    std::vector<int> countTypesInCell(int row, int col) const;

 private:

    const int types;    // number of particle types
    const int size;     // number of cells in one dimension
    const float bounds; // in world grid bounds

    std::vector<std::vector<std::vector<Particle*>>> grid; // [size+2][size+2], corner and edge wraps
    
    std::vector<std::vector<Particle>> cornerWraps; // TL, TR, BR, BL

    std::vector<std::vector<std::vector<Particle>>> edgeWraps;   // T[size],  R[size],  B[size],  L[size]
    
};


#endif