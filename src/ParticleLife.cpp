#include "ParticleLife.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <vector>
#include <cmath>
#include <iostream>


ParticleLife::ParticleLife(Settings settings) :
    types       (settings.types),
    size        (settings.size),
    bounds      (nextafterf(2 * settings.size, 0.0f)),
    count       (settings.count),
    resistance  (settings.resistance),
    innerRadius (settings.innerRadius),
    step        (settings.step),
    attractions (settings.attractions),
    spatialHash (settings.size, settings.types)
{
    SetRandomSeed(settings.seed);

    particles.resize(count);
    for (Particle& p : particles) {
        p.type = GetRandomValue(0, types-1);
        p.pos = Vector2 { GetRandomValue(0, 100*bounds) / 100.0f, GetRandomValue(0, 100*bounds) / 100.0f };
        p.vel = Vector2 { 0, 0 };
    }

    Image temp = GenImageColor(64, 64, BLANK);
    ImageDrawCircle(&temp, 32, 32, 32, WHITE);
    particleTexture = LoadTextureFromImage(temp);
    UnloadImage(temp);
}

ParticleLife::~ParticleLife()
{
    UnloadTexture(particleTexture);
}


void ParticleLife::update()
{
    spatialHash.map(particles);
    
    for (int i = 0; i < count; i++) {
        Particle& p1 = particles[i];

        for (int j = 0; j < count; j++) {
            if (i == j) continue;

            Particle& p2 = particles[j];

            particleInteraction(p1, p2);

        }
    }

    const float invResistance = 1.0f - resistance;

    for (Particle& p : particles) {

        p.vel.x *= invResistance;
        p.vel.y *= invResistance;

        p.pos.x += step * p.vel.x;
        p.pos.y += step * p.vel.y;

        if (p.pos.x < 0.0f)         p.pos.x = 0.0f,   p.vel.x *= -1.0f;
        else if (p.pos.x > bounds)  p.pos.x = bounds, p.vel.x *= -1.0f;
        if (p.pos.y < 0.0f)         p.pos.y = 0.0f,   p.vel.y *= -1.0f;
        else if (p.pos.y > bounds)  p.pos.y = bounds, p.vel.y *= -1.0f;
    }
}


void ParticleLife::draw() const
{
    rlSetTexture(particleTexture.id);
    rlBegin(RL_QUADS);

        for (Particle const& p : particles) {
            Color colour = particleColors[p.type];
            rlColor4ub(colour.r, colour.g, colour.b, colour.a);
            rlNormal3f(0.0f, 0.0f, 1.0f);

            rlTexCoord2f(0.0f, 0.0f); rlVertex2f(p.pos.x-0.05f, p.pos.y-0.05f);
            rlTexCoord2f(0.0f, 1.0f); rlVertex2f(p.pos.x-0.05f, p.pos.y+0.05f);
            rlTexCoord2f(1.0f, 1.0f); rlVertex2f(p.pos.x+0.05f, p.pos.y+0.05f);
            rlTexCoord2f(1.0f, 0.0f); rlVertex2f(p.pos.x+0.05f, p.pos.y-0.05f);
        }

    rlSetTexture(0);
    rlEnd();
}

void ParticleLife::printCell(int row, int col)
{
    // get the cell and the type counts
    std::vector<Particle*>& cell = spatialHash[row][col];
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
