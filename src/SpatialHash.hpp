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

    const int types;
    const int size;
    const float bounds;

    std::vector<std::vector<std::vector<Particle*>>> grid;
    std::vector<std::vector<Particle>> cornerWraps;
    std::vector<std::vector<Particle>> edgeWraps;
    
};


#endif