#ifndef SPATIALHASH_HPP
#define SPATIALHASH_HPP

#include "Particle.hpp"

#include <raylib.h>
#include <vector>
#include <iostream>


#define taureanEdgeIndex(name) inline const unsigned int name(unsigned int c) const



class SpatialHash
{
  public:

    SpatialHash(unsigned int size, vector<Particle>& particles);

    void map();  // map particles to taureanGrid

    vector<Particle*>& getCell(unsigned int row, unsigned int col);

    const vector<const vector<Particle>*> getWrapCellPtrs() const;

  private:

    const unsigned int S;   // number of cells in one dimension
    const float B;

    vector<Particle>& particles;

    vector<vector<vector<Particle*>>> taureanGrid;
    
    vector<vector<Particle>> cornerWrapCells;        // TL, TR, BR, BL

    vector<vector<vector<Particle>>> edgeWrapCells;  // T[size],  R[size],  B[size],  L[size]

    unsigned int hash(float coord) const;            // hash y for row, x for col

    const unsigned int  inCornerIdx[4][2] = { {   S,   S }, {   S,   1 }, {   1,   1 }, {   1,   S } };
    const unsigned int outCornerIdx[4][2] = { {   0,   0 }, {   0, S+1 }, { S+1, S+1 }, { S+1,   0 } };
    const float         xyCornerOff[4][2] = { {  -B,  -B }, {   B,  -B }, {   B,   B }, {  -B,   B } };

    taureanEdgeIndex(inEdgeRow0) { return S;  }  taureanEdgeIndex(inEdgeCol0) { return c;  }
    taureanEdgeIndex(inEdgeRow1) { return c;  }  taureanEdgeIndex(inEdgeCol1) { return 1U; }
    taureanEdgeIndex(inEdgeRow2) { return 1U; }  taureanEdgeIndex(inEdgeCol2) { return c;  }
    taureanEdgeIndex(inEdgeRow3) { return c;  }  taureanEdgeIndex(inEdgeCol3) { return S;  }

    taureanEdgeIndex(outEdgeRow0) { return 0U;  }  taureanEdgeIndex(outEdgeCol0) { return c;   }
    taureanEdgeIndex(outEdgeRow1) { return c;   }  taureanEdgeIndex(outEdgeCol1) { return S+1; }
    taureanEdgeIndex(outEdgeRow2) { return S+1; }  taureanEdgeIndex(outEdgeCol2) { return c;   }
    taureanEdgeIndex(outEdgeRow3) { return c;   }  taureanEdgeIndex(outEdgeCol3) { return 0U;  }    

    inline void offsetEdgeParticle0(Particle& p) const { p.pos.y -= B; }
    inline void offsetEdgeParticle1(Particle& p) const { p.pos.x += B; }
    inline void offsetEdgeParticle2(Particle& p) const { p.pos.y += B; }
    inline void offsetEdgeParticle3(Particle& p) const { p.pos.x -= B; }

    using tareanEdgeFuncPtr = const unsigned int (SpatialHash::*)(unsigned int) const;
    using offsetParticleFuncPtr = void (SpatialHash::*)(Particle&) const;

    tareanEdgeFuncPtr inEdgeIdx[4][2] = { { &SpatialHash::inEdgeRow0, &SpatialHash::inEdgeCol0 }, 
                                          { &SpatialHash::inEdgeRow1, &SpatialHash::inEdgeCol1 },
                                          { &SpatialHash::inEdgeRow2, &SpatialHash::inEdgeCol2 },
                                          { &SpatialHash::inEdgeRow3, &SpatialHash::inEdgeCol3 }, };

    tareanEdgeFuncPtr outEdgeIdx[4][2] = { { &SpatialHash::outEdgeRow0, &SpatialHash::outEdgeCol0 }, 
                                           { &SpatialHash::outEdgeRow1, &SpatialHash::outEdgeCol1 },
                                           { &SpatialHash::outEdgeRow2, &SpatialHash::outEdgeCol2 },
                                           { &SpatialHash::outEdgeRow3, &SpatialHash::outEdgeCol3 }, };

    offsetParticleFuncPtr offsetEdgeParticle[4] = { &SpatialHash::offsetEdgeParticle0,
                                                    &SpatialHash::offsetEdgeParticle1,
                                                    &SpatialHash::offsetEdgeParticle2,
                                                    &SpatialHash::offsetEdgeParticle3, };

};


#endif