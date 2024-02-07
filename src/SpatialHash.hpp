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

    void map(std::vector<Particle>& particles);

    std::vector<int> countTypesInCell(int row, int col) const;


    std::vector<std::vector<Particle*>>& operator[](int index);

    friend std::ostream& operator << (std::ostream& os, const SpatialHash& spatHash);

 private:

    const int types;

    std::vector<std::vector<std::vector<Particle*>>> grid;


    int hash(float coord);
    
};


#endif