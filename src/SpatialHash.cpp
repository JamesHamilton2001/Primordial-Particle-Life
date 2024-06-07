#include "SpatialHash.hpp"

#include "Settings.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <vector>
#include <cmath>
#include <functional>
#include <iostream>


SpatialHash::SpatialHash(unsigned int size, vector<Particle>& particles) :
    S(size),
    B(PARTICLELIFE_DIAMETER * size),
    particles(particles),
    taureanGrid(size+2, vector<vector<Particle*>>(size+2, vector<Particle*>())),
    cornerWrapCells(4, vector<Particle>()),
    edgeWrapCells(4, vector<vector<Particle>>(size))
{}



void SpatialHash::map()
{
    // clear cells in taureanGrid, cornerWrapCells, and edgeWrapCells
    for (auto& row : taureanGrid) for (auto& cell : row) cell.clear();
    for (auto& corner : cornerWrapCells) corner.clear();
    for (auto& edge : edgeWrapCells) for (auto& cell : edge) cell.clear();

    // euclidean mapping

    for (Particle& p : particles)
        taureanGrid[hash(p.pos.y)][hash(p.pos.x)].emplace_back(&p);

    // taurean corners

    for (unsigned int i = 0; i < 4; i++) {
        for (Particle* ptr : taureanGrid[inCornerIdx[i][0]][inCornerIdx[i][1]]) {
            Particle p = *ptr;
            p.pos.x += xyCornerOff[i][0];
            p.pos.y += xyCornerOff[i][1];
            cornerWrapCells[i].emplace_back(p);
        }for (Particle& p : cornerWrapCells[i])
            taureanGrid[outCornerIdx[i][0]][outCornerIdx[i][1]].emplace_back(&p);
    }

    // taurean edges

    for (unsigned int r = 0; r < 4; r++) {
        for (unsigned int c = 1; c <= S; c++) {
            for (Particle* ptr : taureanGrid[(this->*inEdgeIdx[r][0])(c)]
                                            [(this->*inEdgeIdx[r][1])(c)]) {
                Particle p = *ptr;
                (this->*offsetEdgeParticle[r])(p);
                edgeWrapCells[r][c-1].emplace_back(p);
            }
        }for (unsigned int c = 1; c <= S; c++)
            for (Particle& p : edgeWrapCells[r][c-1])
                taureanGrid[(this->*outEdgeIdx[r][0])(c)]
                           [(this->*outEdgeIdx[r][1])(c)].emplace_back(&p);
    }
}

vector<Particle*>& SpatialHash::getCell(unsigned int row, unsigned int col)
{
    return taureanGrid[row][col];
}

const vector<const vector<Particle>*> SpatialHash::getWrapCellPtrs() const
{
    vector<const vector<Particle>*> cellPtrs;
    for (const auto& corner : cornerWrapCells) cellPtrs.push_back(&corner);
    for (const auto& edge : edgeWrapCells) 
        for (const auto& cell : edge) 
            cellPtrs.push_back(&cell);
    return cellPtrs;
}

unsigned int SpatialHash::hash(float coord) const
{
    return coord / PARTICLELIFE_DIAMETER + 1;
}