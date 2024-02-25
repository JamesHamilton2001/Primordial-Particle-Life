#include "SpatialHash.hpp"

#include <raylib.h>
#include <vector>
#include <iostream>


SpatialHash::SpatialHash(int size, int types) :
    types(types)
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

std::vector<Particle*> SpatialHash::cellAtPos(Vector2 pos)
{
    return grid[hash(pos.y)][hash(pos.x)];
}

std::vector<int> SpatialHash::countTypesInCell(int row, int col) const
{
    std::vector<int> counts(types, 0);

    for (Particle* p : grid[row][col])
        counts[p->type]++;

    return counts;
}

std::vector<std::vector<Particle*>>& SpatialHash::operator[](int index)
{
    return grid[index];
}

