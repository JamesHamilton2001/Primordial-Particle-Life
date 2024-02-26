#include "SpatialHash.hpp"

#include <raylib.h>
#include <vector>
#include <cmath>
#include <iostream>


SpatialHash::SpatialHash(int size, int types) :
    types(types),
    size(size),
    bounds(nextafterf(2 * size, 0.0f)),
    grid(size+2, std::vector<std::vector<Particle*>>(size+2, std::vector<Particle*>())),
    cornerWraps(4, std::vector<Particle>()),
    edgeWraps(4, std::vector<Particle>())
{

}


int SpatialHash::hash(float coord) const
{
    return coord/2 + 1;
}

void SpatialHash::map(std::vector<Particle>& particles)
{
    // clear cells in grid, cornerWraps, and edgeWraps
    for (auto& row : grid) for (auto& cell : row) cell.clear();
    for (auto& cell : cornerWraps) cell.clear();
    for (auto& cell : edgeWraps) cell.clear();

    // map normal particles to grid
    for (Particle& p : particles)
        grid[hash(p.pos.y)][hash(p.pos.x)].emplace_back(&p);

    // BR wraps to TL
    for (Particle* ptr : grid[size][size]) {    // TL <- BR : ↖  DUN
        Particle p = *ptr; 
        p.pos.x -= bounds;
        p.pos.y -= bounds;
        cornerWraps[0].emplace_back(p);
    } for (Particle& p : cornerWraps[0]) grid[0][0].emplace_back(&p);
    // BL wraps to TR
    for (Particle* ptr : grid[size][1]) {       // TR <- BL : ↗  DUN
        Particle p = *ptr;
        p.pos.x += bounds;
        p.pos.y -= bounds;
        cornerWraps[1].emplace_back(p);
    } for (Particle& p : cornerWraps[1]) grid[0][size+1].emplace_back(&p);
    // TL wraps to BR
    for (Particle* ptr : grid[1][1]) {          // BR <- TL : ↘  DUN
        Particle p = *ptr;
        p.pos.x += bounds;
        p.pos.y += bounds;
        cornerWraps[2].emplace_back(p);
    } for (Particle& p : cornerWraps[2]) grid[size+1][size+1].emplace_back(&p);
    // TR wraps to BL
    for (Particle* ptr : grid[1][size]) {       // BL <- TR : ↙  DUN
        Particle p = *ptr;
        p.pos.x -= bounds;
        p.pos.y += bounds;
        cornerWraps[3].emplace_back(p);
    } for (Particle& p : cornerWraps[3]) grid[size+1][0].emplace_back(&p);


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
