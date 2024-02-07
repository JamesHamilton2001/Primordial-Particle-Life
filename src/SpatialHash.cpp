#include "SpatialHash.hpp"

#include <raylib.h>
#include <vector>
#include <iostream>


SpatialHash::SpatialHash(int size, int types) :
    types(types)
{
    grid.resize(size);
    for (int i = 0; i < size; i++)
        grid[i].resize(size);
}


void SpatialHash::map(std::vector<Particle>& particles)
{
    for (std::vector<std::vector<Particle*>>& row : grid)
        for (std::vector<Particle*>& cell : row)
            cell.clear();

    for (Particle& p : particles)
        grid[hash(p.pos.x)][hash(p.pos.y)].push_back(&p);
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


std::ostream& operator<<(std::ostream& os, const SpatialHash& spatHash)
{
    for (int r = 0; r < static_cast<int>(spatHash.grid.size()); r++) {
        const std::vector<std::vector<Particle*>>& row = spatHash.grid[r];

        for (int c = 0; c < static_cast<int>(row.size()); c++) {
            const std::vector<Particle*>& cell = row[c];

            std::vector<int> counts = spatHash.countTypesInCell(r, c);

            os <<  "| [" << r <<"][" << c <<
                 "] | (" << cell.size() << "/" << cell.capacity() <<
                 ") | ( ";
            os << counts[0];
            for (int i = 1; i < static_cast<int>(counts.size()); i++)
                os << " : " << counts[i];
            os <<") |" << std::endl;

            for (Particle* p : cell)
                os << "  " << *p << std::endl;
        }
    }
    return os;
}


int SpatialHash::hash(float coord)
{
    return coord / 2;
}

