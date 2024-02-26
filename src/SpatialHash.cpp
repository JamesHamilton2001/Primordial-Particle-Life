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

    // for (Particle& p : particles)
    //     grid[hash(p.pos.y)][hash(p.pos.x)].push_back(&p);

    wrappedParticles.clear();

    for (Particle& p : particles) {
        const int row = hash(p.pos.y);
        const int col = hash(p.pos.x);
        grid[row][col].push_back(&p);
    }

    // map corner overlaps
    for (Particle* ptr : grid[1][1]) {
        wrappedParticles.push_back(*ptr);
        wrappedParticles.back().pos.x += bounds;
        wrappedParticles.back().pos.y += bounds;
        grid[size+1][size+1].push_back(&wrappedParticles.back());
        // std::cout << *ptr << " ---> " << wrappedParticles.back() << std::endl;
    } for (Particle* ptr : grid[1][size]) {
        wrappedParticles.push_back(*ptr);
        wrappedParticles.back().pos.x -= bounds;
        wrappedParticles.back().pos.y += bounds;
        grid[size+1][0].push_back(&wrappedParticles.back());
        // std::cout << *ptr << " ---> " << wrappedParticles.back() << std::endl;
    } for (Particle* ptr : grid[size][1]) {
        wrappedParticles.push_back(*ptr);
        wrappedParticles.back().pos.x += bounds;
        wrappedParticles.back().pos.y -= bounds;
        grid[0][size+1].push_back(&wrappedParticles.back());
        // std::cout << *ptr << " ---> " << wrappedParticles.back() << std::endl;
    } for (Particle* ptr : grid[size][size]) {
        wrappedParticles.push_back(*ptr);
        wrappedParticles.back().pos.x -= bounds;
        wrappedParticles.back().pos.y -= bounds;
        grid[0][0].push_back(&wrappedParticles.back());
        // std::cout << *ptr << " ---> " << wrappedParticles.back() << std::endl;
    }

    // map row and col overlaps
    for (int c = 1; c <= size; c++) {
        for (Particle* ptr : grid[1][c]) {
            wrappedParticles.push_back(*ptr);
            wrappedParticles.back().pos.y += bounds;
            grid[size+1][c].push_back(&wrappedParticles.back());
        } for (Particle* ptr : grid[size][c]) {
            wrappedParticles.push_back(*ptr);
            wrappedParticles.back().pos.y -= bounds;
            grid[0][c].push_back(&wrappedParticles.back());
        }
    } for (int r = 1; r <= size; r++) {
        for (Particle* ptr : grid[r][1]) {
            wrappedParticles.push_back(*ptr);
            wrappedParticles.back().pos.x += bounds;
            grid[r][size+1].push_back(&wrappedParticles.back());
        } for (Particle* ptr : grid[r][size]) {
            wrappedParticles.push_back(*ptr);
            wrappedParticles.back().pos.x -= bounds;
            grid[r][0].push_back(&wrappedParticles.back());
        }
    }
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
