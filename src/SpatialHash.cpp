#include "SpatialHash.hpp"

#include <raylib.h>
#include <vector>
#include <cmath>
#include <iostream>


SpatialHash::SpatialHash(int size, int types) :
    types(types),
    size(size),
    bounds(nextafterf(2 * size, 0.0f))
{
    grid.resize(size+2, std::vector<std::vector<Particle*>>(size+2));
}


int SpatialHash::hash(float coord) const
{
    return coord/2 + 1;
}

void SpatialHash::map(std::vector<Particle>& particles)
{
    for (std::vector<std::vector<Particle*>>& row : grid)
        for (std::vector<Particle*>& cell : row)
            cell.clear();

    for (Particle& p : particles)
        grid[hash(p.pos.y)][hash(p.pos.x)].push_back(&p);

}

std::vector<Particle*>& SpatialHash::getCell(int row, int col)
{
    return grid[row][col];
}

std::vector<int> SpatialHash::countTypesInCell(int row, int col) const
{
    std::vector<int> counts(types, 0);

    for (Particle* p : grid[row][col])
        counts[p->type]++;

    return counts;
}
