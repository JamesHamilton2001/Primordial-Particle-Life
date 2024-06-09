#ifndef SPATIALHASH_HPP
#define SPATIALHASH_HPP

#include "Particle.hpp"

#include <raylib.h>
#include <vector>
#include <iostream>


#define taureanEdgeIndex(name) inline const unsigned int name(unsigned int c) const

#define taureanEdgeIdx(name, ret) inline const unsigned int name(unsigned int c) const { return ret; }

class SpatialHash
{
  public:

    SpatialHash(unsigned int size, vector<Particle>& particles);

    void map();  // map particles to taureanGrid

    vector<Particle*>& getCell(unsigned int row, unsigned int col);

    const vector<const vector<Particle>*> getWrapCellPtrs() const;

  private:


    const unsigned int S;   // (size) number of cells in one dimension
    const float B;          // (bounds) size of simulation space in units
 
    vector<Particle>& particles;

    vector<vector<vector<Particle*>>> taureanGrid;

    vector<vector<Particle>> cornerWrapCells;        // TL, TR, BR, BL

    vector<vector<vector<Particle>>> edgeWrapCells;  // T[size],  R[size],  B[size],  L[size]

    unsigned int hash(float coord) const;            // hash y for row, x for col

    const unsigned int  inCornerIdx[4][2] = { {   S,   S }, {   S,   1 }, {   1,   1 }, {   1,   S } };
    const unsigned int outCornerIdx[4][2] = { {   0,   0 }, {   0, S+1 }, { S+1, S+1 }, { S+1,   0 } };
    const float         xyCornerOff[4][2] = { {  -B,  -B }, {   B,  -B }, {   B,   B }, {  -B,   B } };

    taureanEdgeIdx(innerEdgeRow0, S)    taureanEdgeIdx(innerEdgeCol0, c)    // func name, return value, one uint arg
    taureanEdgeIdx(innerEdgeRow1, c)    taureanEdgeIdx(innerEdgeCol1, 1)
    taureanEdgeIdx(innerEdgeRow2, 1)    taureanEdgeIdx(innerEdgeCol2, c)
    taureanEdgeIdx(innerEdgeRow3, c)    taureanEdgeIdx(innerEdgeCol3, S)

    taureanEdgeIdx(outerEdgeRow0,   0)  taureanEdgeIdx(outerEdgeCol0,   c)
    taureanEdgeIdx(outerEdgeRow1,   c)  taureanEdgeIdx(outerEdgeCol1, S+1)
    taureanEdgeIdx(outerEdgeRow2, S+1)  taureanEdgeIdx(outerEdgeCol2,   c)
    taureanEdgeIdx(outerEdgeRow3,   c)  taureanEdgeIdx(outerEdgeCol3,   0)

    inline void offsetEdgeParticle0(Particle& p) const { p.pos.y -= B; }
    inline void offsetEdgeParticle1(Particle& p) const { p.pos.x += B; }
    inline void offsetEdgeParticle2(Particle& p) const { p.pos.y += B; }
    inline void offsetEdgeParticle3(Particle& p) const { p.pos.x -= B; }

    using tareanEdgeFuncPtr = const unsigned int (SpatialHash::*)(unsigned int) const;
    using offsetParticleFuncPtr = void (SpatialHash::*)(Particle&) const;

    tareanEdgeFuncPtr innerEdgeIdx[4][2] = { { &SpatialHash::innerEdgeRow0, &SpatialHash::innerEdgeCol0 }, 
                                             { &SpatialHash::innerEdgeRow1, &SpatialHash::innerEdgeCol1 },
                                             { &SpatialHash::innerEdgeRow2, &SpatialHash::innerEdgeCol2 },
                                             { &SpatialHash::innerEdgeRow3, &SpatialHash::innerEdgeCol3 }, };

    tareanEdgeFuncPtr outerEdgeIdx[4][2] = { { &SpatialHash::outerEdgeRow0, &SpatialHash::outerEdgeCol0 }, 
                                             { &SpatialHash::outerEdgeRow1, &SpatialHash::outerEdgeCol1 },
                                             { &SpatialHash::outerEdgeRow2, &SpatialHash::outerEdgeCol2 },
                                             { &SpatialHash::outerEdgeRow3, &SpatialHash::outerEdgeCol3 }, };

    offsetParticleFuncPtr offsetEdgeParticle[4] = { &SpatialHash::offsetEdgeParticle0,
                                                    &SpatialHash::offsetEdgeParticle1,
                                                    &SpatialHash::offsetEdgeParticle2,
                                                    &SpatialHash::offsetEdgeParticle3, };

};


#endif