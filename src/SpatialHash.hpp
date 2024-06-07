#ifndef SPATIALHASH_HPP
#define SPATIALHASH_HPP

#include "Particle.hpp"

#include <raylib.h>
#include <vector>
#include <iostream>


class SpatialHash
{
  public:

    SpatialHash(unsigned int size, vector<Particle>& particles);

    void map();  // map particles to taureanGrid

    vector<Particle*>& getCell(unsigned int row, unsigned int col);

    const vector<const vector<Particle>*> getWrapCellPtrs() const;

  private:

    const unsigned int size;   // number of cells in one dimension
    const float bounds;

    vector<Particle>& particles;

    vector<vector<vector<Particle*>>> taureanGrid;
    
    vector<vector<Particle>> cornerWrapCells;        // TL, TR, BR, BL

    vector<vector<vector<Particle>>> edgeWrapCells;  // T[size],  R[size],  B[size],  L[size]

    unsigned int hash(float coord) const;   // hash y for row, x for col

    const unsigned int  inCornerIdx[4][2] = { {   size,   size}, {   size,      1}, {      1,      1}, {      1,   size} };
    const unsigned int outCornerIdx[4][2] = { {      0,      0}, {      0, size+1}, { size+1, size+1}, { size+1,      0} };
    const float         xyCornerOff[4][2] = { {-bounds,-bounds}, { bounds,-bounds}, { bounds, bounds}, {-bounds, bounds} };

    inline const unsigned int inEdgeRow0(unsigned int c) const { return size; }
    inline const unsigned int inEdgeRow1(unsigned int c) const { return c;    }
    inline const unsigned int inEdgeRow2(unsigned int c) const { return 1U;   }
    inline const unsigned int inEdgeRow3(unsigned int c) const { return c;    }

    inline const unsigned int inEdgeCol0(unsigned int c) const { return c;    }
    inline const unsigned int inEdgeCol1(unsigned int c) const { return 1U;   }
    inline const unsigned int inEdgeCol2(unsigned int c) const { return c;    }
    inline const unsigned int inEdgeCol3(unsigned int c) const { return size; }

    inline const unsigned int outEdgeRow0(unsigned int c) const { return 0U;     }
    inline const unsigned int outEdgeRow1(unsigned int c) const { return c;      }
    inline const unsigned int outEdgeRow2(unsigned int c) const { return size+1; }
    inline const unsigned int outEdgeRow3(unsigned int c) const { return c;      }

    inline const unsigned int outEdgeCol0(unsigned int c) const { return c;      }
    inline const unsigned int outEdgeCol1(unsigned int c) const { return size+1; }
    inline const unsigned int outEdgeCol2(unsigned int c) const { return c;      }
    inline const unsigned int outEdgeCol3(unsigned int c) const { return 0U;     }

    inline void offsetEdgeParticle0(Particle& p) const { p.pos.y -= bounds; }
    inline void offsetEdgeParticle1(Particle& p) const { p.pos.x += bounds; }
    inline void offsetEdgeParticle2(Particle& p) const { p.pos.y += bounds; }
    inline void offsetEdgeParticle3(Particle& p) const { p.pos.x -= bounds; }

    const unsigned int (SpatialHash::*inEdgeIdx[4][2])(unsigned int) const = {
        { &SpatialHash::inEdgeRow0, &SpatialHash::inEdgeCol0 }, 
        { &SpatialHash::inEdgeRow1, &SpatialHash::inEdgeCol1 },
        { &SpatialHash::inEdgeRow2, &SpatialHash::inEdgeCol2 },
        { &SpatialHash::inEdgeRow3, &SpatialHash::inEdgeCol3 },
    };
    const unsigned int (SpatialHash::*outEdgeIdx[4][2])(unsigned int) const = {
        { &SpatialHash::outEdgeRow0, &SpatialHash::outEdgeCol0 }, 
        { &SpatialHash::outEdgeRow1, &SpatialHash::outEdgeCol1 },
        { &SpatialHash::outEdgeRow2, &SpatialHash::outEdgeCol2 },
        { &SpatialHash::outEdgeRow3, &SpatialHash::outEdgeCol3 },
    };
    void (SpatialHash::*offsetEdgeParticle[4])(Particle&) const = {
        &SpatialHash::offsetEdgeParticle0,
        &SpatialHash::offsetEdgeParticle1,
        &SpatialHash::offsetEdgeParticle2,
        &SpatialHash::offsetEdgeParticle3,
    };
};


#endif