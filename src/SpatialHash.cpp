#include "SpatialHash.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <vector>
#include <cmath>
#include <iostream>


SpatialHash::SpatialHash(unsigned int size, unsigned int types) :
    types(types),
    size(size),
    bounds(2.0f * size),
    grid(size+2, vector<vector<Particle*>>(size+2, vector<Particle*>())),
    cornerWrapCells(4, vector<Particle>()),
    edgeWrapCells(4, vector<vector<Particle>>(size)),
    allWrapCellPtrs([&]() {
        vector<vector<Particle>*> cellPtrs;
        for (auto& corner : cornerWrapCells) cellPtrs.emplace_back(&corner);
        for (auto& edge : edgeWrapCells) for (auto& cell : edge) cellPtrs.emplace_back(&cell);
        return cellPtrs;
    }())
{}


unsigned int SpatialHash::hash(float coord) const
{
    return coord/2 + 1;
}

void SpatialHash::map(vector<Particle>& particles)
{
    // clear cells in grid, cornerWrapCells, and edgeWrapCells
    for (auto& row : grid) for (auto& cell : row) cell.clear();
    for (auto& corner : cornerWrapCells) corner.clear();
    for (auto& edge : edgeWrapCells) for (auto& cell : edge) cell.clear();

    // map normal particles to grid
    for (Particle& p : particles)
        grid[hash(p.pos.y)][hash(p.pos.x)].emplace_back(&p);

    // BR wraps to TL
    for (Particle* ptr : grid[size][size]) {    // TL <- BR : ↖ 
        Particle p = *ptr; 
        p.pos.x -= bounds;
        p.pos.y -= bounds;
        cornerWrapCells[0].emplace_back(p);
    } for (Particle& p : cornerWrapCells[0]) grid[0][0].emplace_back(&p);
    // BL wraps to TR
    for (Particle* ptr : grid[size][1]) {       // TR <- BL : ↗ 
        Particle p = *ptr;
        p.pos.x += bounds;
        p.pos.y -= bounds;
        cornerWrapCells[1].emplace_back(p);
    } for (Particle& p : cornerWrapCells[1]) grid[0][size+1].emplace_back(&p);
    // TL wraps to BR
    for (Particle* ptr : grid[1][1]) {          // BR <- TL : ↘ 
        Particle p = *ptr;
        p.pos.x += bounds;
        p.pos.y += bounds;
        cornerWrapCells[2].emplace_back(p);
    } for (Particle& p : cornerWrapCells[2]) grid[size+1][size+1].emplace_back(&p);
    // TR wraps to BL
    for (Particle* ptr : grid[1][size]) {       // BL <- TR : ↙ 
        Particle p = *ptr;
        p.pos.x -= bounds;
        p.pos.y += bounds;
        cornerWrapCells[3].emplace_back(p);
    } for (Particle& p : cornerWrapCells[3]) grid[size+1][0].emplace_back(&p);

    // B wraps to T
    for (unsigned int c = 1; c <= size; c++) {
        for (Particle* ptr : grid[size][c]) {   // T <- B : ↑
            Particle p = *ptr;
            p.pos.y -= bounds;
            edgeWrapCells[0][c-1].emplace_back(p);
        }
    } for (unsigned int c = 1; c <= size; c++)
        for (Particle& p : edgeWrapCells[0][c-1]) grid[0][c].emplace_back(&p);
    // L wraps to R
    for (unsigned int r = 1; r <= size; r++) {
        for (Particle* ptr : grid[r][1]) {      // R <- L : →
            Particle p = *ptr;
            p.pos.x += bounds;
            edgeWrapCells[1][r-1].emplace_back(p);
        }
    } for (unsigned int r = 1; r <= size; r++)
        for (Particle& p : edgeWrapCells[1][r-1]) grid[r][size+1].emplace_back(&p);
    // T wraps to B
    for (unsigned int c = 1; c <= size; c++) {
        for (Particle* ptr : grid[1][c]) {      // B <- T : ↓
            Particle p = *ptr;
            p.pos.y += bounds;
            edgeWrapCells[2][c-1].emplace_back(p);
        }
    } for (unsigned int c = 1; c <= size; c++)
        for (Particle& p : edgeWrapCells[2][c-1]) grid[size+1][c].emplace_back(&p);
    // R wraps to L
    for (unsigned int r = 1; r <= size; r++) {
        for (Particle* ptr : grid[r][size]) {   // L <- R : ←
            Particle p = *ptr;
            p.pos.x -= bounds;
            edgeWrapCells[3][r-1].emplace_back(p);
        }
    } for (unsigned int r = 1; r <= size; r++)
        for (Particle& p : edgeWrapCells[3][r-1]) grid[r][0].emplace_back(&p);
}

vector<Particle*>& SpatialHash::getCell(unsigned int row, unsigned int col)
{
    return grid[row][col];
}

// const vector<vector<Particle>*> SpatialHash::getWrapCellPtrs() const
// {
//     return vector<vector<Particle>*>([&]() {
//         vector<vector<Particle>*> cellPtrs;
//         for (const auto& corner : cornerWrapCells) cellPtrs.emplace_back(&corner);
//         for (const auto& edge : edgeWrapCells) for (const auto& cell : edge) cellPtrs.emplace_back(&cell);
//         return cellPtrs;
//     }());
// }
