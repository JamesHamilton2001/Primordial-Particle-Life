#include "ParticleLife.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>


ParticleLife::ParticleLife(Settings settings) :
    types       (settings.types),
    size        (settings.size),
    bounds      (2.0f * settings.size),
    count       (settings.count),
    resistance  (settings.resistance),
    innerRadius (settings.innerRadius),
    step        (settings.step),
    attractions (settings.attractions),
    spatialHash (settings.size, settings.types)
{
    posDistr = std::uniform_real_distribution<float>(0.0f, bounds);
    gen.seed(settings.seed);

    particles.resize(count);
    randomisePositions();

}

ParticleLife::~ParticleLife()
{}


void ParticleLife::update()
{
    // for (Particle& p1 : particles)
    //     for (Particle& p2 : particles)
    //         if (&p1 != &p2)
    //             particleInteraction(p1, p2);

    // TODO: spatial hash does not yet produce correct results
    spatialHash.map(particles);
    debugGrid();

    // for each cell...
    for (int row = 1; row <= size; row++) {
        for (int col = 1; col <= size; col++) {
            auto& cell = spatialHash.getCell(row, col);

            // ...and each neighbour (including itself)
            for (int r = row-1; r <= row+1; r++) {
                for (int c = col-1; c <= col+1; c++) {
                    auto& neighbour = spatialHash.getCell(r, c);
                    
                    // interact particles within neighbourhood
                    for (Particle* p1 : cell)
                        for (Particle* p2 : neighbour)
                            if (p1 != p2)
                                particleInteraction(*p1, *p2);
                }
            }
        }
    }

    // for each particle, apply resistance, update position and wrap position
    const float invResistance = 1.0f - resistance;
    for (Particle& p : particles) {

        p.vel.x *= invResistance;
        p.vel.y *= invResistance;
        
        p.pos.x += step * p.vel.x;
        p.pos.y += step * p.vel.y;

        if (p.pos.x < 0.0f)         p.pos.x += bounds;
        else if (p.pos.x > bounds)  p.pos.x -= bounds;
        if (p.pos.y < 0.0f)         p.pos.y += bounds;
        else if (p.pos.y > bounds)  p.pos.y -= bounds;
    }
}


void ParticleLife::draw(unsigned int pTexID) const
{
    rlSetTexture(pTexID);
    rlBegin(RL_QUADS);
        for (Particle const& p : particles)
            p.draw();
    rlSetTexture(0);
    rlEnd();
}

void ParticleLife::drawSoftBorder() const
{
    const float b = bounds;

    rlSetTexture(1);
    rlBegin(RL_QUADS);
        rlNormal3f(0.0f, 0.0f, 1.0f);
        rlColor4ub(0,0,0,255);

        // top left corner
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(-2, -2);   // top left
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(-2, 0);    // bottom left
        rlColor4ub(0,0,0,0);
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(0, 0);     // bottom right
        rlColor4ub(0,0,0,255);
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(0,-2);     // top right

        // top right corner
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(b+2, -2);  // top right
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(b, -2);    // top left
        rlColor4ub(0,0,0,0);
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(b, 0);     // bottom left
        rlColor4ub(0,0,0,255);
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(b+2, 0);   // bottom right
        
        // bottom right corner
        rlColor4ub(0,0,0,0);
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(b, b);     // top left
        rlColor4ub(0,0,0,255);
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(b, b+2);   // bottom left
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(b+2, b+2); // bottom right
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(b+2, b);   // top right

        // bottom left corner
        rlColor4ub(0,0,0,0);
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(0, b);     // top right
        rlColor4ub(0,0,0,255);
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(-2, b);    // top left
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(-2, b+2);  // bottom left
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(0, b+2);   // bottom right

    rlEnd();
    rlSetTexture(0);

    // edges
    DrawRectangleGradientV(0, -2, b, 2, BLACK, BLANK); // top
    DrawRectangleGradientV(0, b, b, 2, BLANK, BLACK);  // bottom
    DrawRectangleGradientH(-2, 0, 2, b, BLACK, BLANK); // left
    DrawRectangleGradientH(b, 0, 2, b, BLANK, BLACK);  // right

    // solid outside border
    DrawRectangle(-3, -3, b+6, 1, BLACK);  // top
    DrawRectangle(-3, b+2, b+6, 1, BLACK); // bottom
    DrawRectangle(-3, -3, 1, b+6, BLACK);  // left
    DrawRectangle(b+2, -3, 1, b+6, BLACK); // right
    
}


void ParticleLife::randomisePositions()
{
    for (Particle& p : particles) {
        p.type = GetRandomValue(0, types-1);
        p.pos = Vector2 { posDistr(gen), posDistr(gen) };
        p.vel = Vector2 { 0.0f, 0.0f };
    }
    std::cout << "finsihsed randomising" << std::endl;
}

void ParticleLife::printCell(int row, int col)
{
    // get the cell and the type counts
    std::vector<Particle*>& cell = spatialHash.getCell(row, col);
    std::vector<int> typeCounts = spatialHash.countTypesInCell(row, col);

    // print cell index, type count ratio, count and capacity
    std::cout << "Cell["<< row <<"]["<< col <<"] " << typeCounts[0];
    for (int i = 1; i < types; i++)
        std::cout << ":" << typeCounts[i];
    std::cout <<" "<< cell.size() <<"/"<< cell.capacity() << std::endl; 

    // print the particles in the cell
    for (Particle* pPtr : cell)
        std::cout << *pPtr << std::endl;
    std::cout << std::endl;
}

void ParticleLife::printCellAtPos(Vector2 pos)
{
    printCell(spatialHash.hash(pos.y), spatialHash.hash(pos.x));
}

std::ostream& operator << (std::ostream& os, const ParticleLife& particleLife)
{
    return os << "| Particle Life | " << std::endl;
}


void ParticleLife::particleInteraction(Particle& p1, Particle& p2)
{
    const float dx = p2.pos.x - p1.pos.x;
    const float dy = p2.pos.y - p1.pos.y;
    const float sqDist = dx*dx + dy*dy;

    if (sqDist <= 4.0f) {
        const float dist = sqrt(sqDist);

        const float coef = (dist <= innerRadius)
            ? 1.0f - innerRadius / dist
            : attractions[p1.type][p2.type] * (dist - innerRadius) / 2.0f;

        p1.vel.x += coef * (dx / dist);
        p1.vel.y += coef * (dy / dist);
    }
}

void ParticleLife::debugGrid()
{

}