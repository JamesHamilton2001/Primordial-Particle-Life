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
    cornerWraps(4, vector<Particle>()),
    edgeWraps(4, vector<vector<Particle>>(size))
{}


unsigned int SpatialHash::hash(float coord) const
{
    return coord/2 + 1;
}

void SpatialHash::map(vector<Particle>& particles)
{
    // clear cells in grid, cornerWraps, and edgeWraps
    for (auto& row : grid) for (auto& cell : row) cell.clear();
    for (auto& corner : cornerWraps) corner.clear();
    for (auto& edge : edgeWraps) for (auto& cell : edge) cell.clear();

    // map normal particles to grid
    for (Particle& p : particles)
        grid[hash(p.pos.y)][hash(p.pos.x)].emplace_back(&p);

    // BR wraps to TL
    for (Particle* ptr : grid[size][size]) {    // TL <- BR : ↖ 
        Particle p = *ptr; 
        p.pos.x -= bounds;
        p.pos.y -= bounds;
        cornerWraps[0].emplace_back(p);
    } for (Particle& p : cornerWraps[0]) grid[0][0].emplace_back(&p);
    // BL wraps to TR
    for (Particle* ptr : grid[size][1]) {       // TR <- BL : ↗ 
        Particle p = *ptr;
        p.pos.x += bounds;
        p.pos.y -= bounds;
        cornerWraps[1].emplace_back(p);
    } for (Particle& p : cornerWraps[1]) grid[0][size+1].emplace_back(&p);
    // TL wraps to BR
    for (Particle* ptr : grid[1][1]) {          // BR <- TL : ↘ 
        Particle p = *ptr;
        p.pos.x += bounds;
        p.pos.y += bounds;
        cornerWraps[2].emplace_back(p);
    } for (Particle& p : cornerWraps[2]) grid[size+1][size+1].emplace_back(&p);
    // TR wraps to BL
    for (Particle* ptr : grid[1][size]) {       // BL <- TR : ↙ 
        Particle p = *ptr;
        p.pos.x -= bounds;
        p.pos.y += bounds;
        cornerWraps[3].emplace_back(p);
    } for (Particle& p : cornerWraps[3]) grid[size+1][0].emplace_back(&p);

    // B wraps to T
    for (unsigned int c = 1; c <= size; c++) {
        for (Particle* ptr : grid[size][c]) {   // T <- B : ↑
            Particle p = *ptr;
            p.pos.y -= bounds;
            edgeWraps[0][c-1].emplace_back(p);
        }
    } for (unsigned int c = 1; c <= size; c++)
        for (Particle& p : edgeWraps[0][c-1]) grid[0][c].emplace_back(&p);
    // L wraps to R
    for (unsigned int r = 1; r <= size; r++) {
        for (Particle* ptr : grid[r][1]) {      // R <- L : →
            Particle p = *ptr;
            p.pos.x += bounds;
            edgeWraps[1][r-1].emplace_back(p);
        }
    } for (unsigned int r = 1; r <= size; r++)
        for (Particle& p : edgeWraps[1][r-1]) grid[r][size+1].emplace_back(&p);
    // T wraps to B
    for (unsigned int c = 1; c <= size; c++) {
        for (Particle* ptr : grid[1][c]) {      // B <- T : ↓
            Particle p = *ptr;
            p.pos.y += bounds;
            edgeWraps[2][c-1].emplace_back(p);
        }
    } for (unsigned int c = 1; c <= size; c++)
        for (Particle& p : edgeWraps[2][c-1]) grid[size+1][c].emplace_back(&p);
    // R wraps to L
    for (unsigned int r = 1; r <= size; r++) {
        for (Particle* ptr : grid[r][size]) {   // L <- R : ←
            Particle p = *ptr;
            p.pos.x -= bounds;
            edgeWraps[3][r-1].emplace_back(p);
        }
    } for (unsigned int r = 1; r <= size; r++)
        for (Particle& p : edgeWraps[3][r-1]) grid[r][0].emplace_back(&p);
}

vector<Particle*>& SpatialHash::getCell(unsigned int row, unsigned int col)
{
    return grid[row][col];
}

void SpatialHash::drawGrid() const
{
    rlBegin(RL_LINES);
        rlColor4ub(80, 80, 80, 255);
        for (int i = -1; i <= static_cast<int>(size)+1; i++)
            rlVertex2f(i*2.0f, -2.0f),  rlVertex2f(i*2.0f, bounds+2.0f),
            rlVertex2f(-2.0f, i*2.0f),  rlVertex2f(bounds+2.0f, i*2.0f);
    rlEnd();
}

void SpatialHash::drawGhosts(unsigned int texID) const
{
    // draw particles in overlapping cells
    rlSetTexture(texID);
    rlBegin(RL_QUADS);
        for (auto const& corner : cornerWraps)
            for (Particle const& p : corner)
                p.draw();
        for (auto const& edge : edgeWraps)
            for (auto const& cell : edge)
                for (Particle const& p : cell)
                    p.draw();
    rlSetTexture(0);
    rlEnd();
}
