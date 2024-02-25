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

    std::vector<Particle*> cellAtPos(Vector2 pos);

    std::vector<int> countTypesInCell(int row, int col) const;

    std::vector<std::vector<Particle*>>& operator [] (int index);

 private:

    const int types;
    std::vector<std::vector<std::vector<Particle*>>> grid;
    
};


#endif