#include "ParticleLife.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>


ParticleLife::ParticleLife(Settings& settings) :
    settings    (settings),
    frameCount  (0),
    types       (settings.types),
    size        (settings.size),
    bounds      (2.0f * settings.size),
    count       (settings.count),
    resistance  (settings.resistance),
    innerRadius (settings.innerRadius),
    step        (settings.step),
    attractions (settings.attractions),
    particles   (settings.particles),
    spatialHash (settings.size, settings.types)
{
    if (settings.seed != -1) {
        SetRandomSeed(settings.seed);
        randomisePositions();
    }
}

void ParticleLife::update()
{
    frameCount++;

    spatialHash.map(particles);

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

    // for each particle
    const float invResistance = 1.0f - resistance;
    for (Particle& p : particles) {

        // apply resistance
        p.vel.x *= invResistance;
        p.vel.y *= invResistance;
        
        // update position
        p.pos.x += step * p.vel.x;
        p.pos.y += step * p.vel.y;

        // wrap arouind bounds
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

        // SOFT top left corner
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(-2, -2);   // top left
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(-2, 0);    // bottom left
        rlColor4ub(0,0,0,0);
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(0, 0);     // bottom right
        rlColor4ub(0,0,0,255);
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(0,-2);     // top right

        // SOFT top right corner
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(b+2, -2);  // top right
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(b, -2);    // top left
        rlColor4ub(0,0,0,0);
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(b, 0);     // bottom left
        rlColor4ub(0,0,0,255);
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(b+2, 0);   // bottom right
        
        // SOFT bottom right corner
        rlColor4ub(0,0,0,0);
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(b, b);     // top left
        rlColor4ub(0,0,0,255);
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(b, b+2);   // bottom left
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(b+2, b+2); // bottom right
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(b+2, b);   // top right

        // SOFT bottom left corner
        rlColor4ub(0,0,0,0);
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(0, b);     // top right
        rlColor4ub(0,0,0,255);
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(-2, b);    // top left
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(-2, b+2);  // bottom left
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(0, b+2);   // bottom right

        // SOFT top edge
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(0, -2);    // top left
        rlColor4ub(0,0,0,0);
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(0, 0);     // bottom left
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(b, 0);     // bottom right
        rlColor4ub(0,0,0,255);
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(b, -2);    // top right

        // SOFT bottom edge
        rlColor4ub(0,0,0,0);
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(0, b);     // top left
        rlColor4ub(0,0,0,255);
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(0, b+2);   // bottom left
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(b, b+2);   // bottom right
        rlColor4ub(0,0,0,0);
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(b, b);     // top right
        rlColor4ub(0,0,0,255);

        // SOFT left edge
        rlColor4ub(0,0,0,255);
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(-2, 0);    // top left
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(-2, b);    // bottom left
        rlColor4ub(0,0,0,0);
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(0, b);     // bottom right
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(0, 0);     // top right

        // SOFT right edge
        rlColor4ub(0,0,0,0);
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(b, 0);     // top left
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(b, b);     // bottom left
        rlColor4ub(0,0,0,255);
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(b+2, b);   // bottom right
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(b+2, 0);   // top right
    
        // HARD top edge
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(-3, -3);   // top left
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(-3, -2);   // bottom left
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(b+3, -2);  // bottom right
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(b+3, -3);  // top right

        // HARD bottom edge
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(-3, b+2);  // top left
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(-3, b+3);  // bottom left
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(b+3, b+3); // bottom right
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(b+3, b+2); // top right

        // HARD left edge
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(-3, -3);   // top left
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(-3, b+3);  // bottom left
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(-2, b+3);  // bottom right
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(-2, -3);   // top right

        // HARD right edge
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(b+2, -3);// top left
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(b+2, b+3);// bottom left
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(b+3, b+3);// bottom right
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(b+3, -3);// top right

    rlEnd();
    rlSetTexture(0);
}

void ParticleLife::drawGrid() const
{
    spatialHash.drawGrid();
}

void ParticleLife::drawGhosts(unsigned int pTexID) const
{
    spatialHash.drawGhosts(pTexID);
}


void ParticleLife::saveConfig() const
{
    // open settings file
    ofstream file(settings.customSettingsDir + settings.name +"("+ to_string(frameCount) +").txt", ofstream::out);
    if (!file.is_open()) {
        throw runtime_error("Failed to open temp config file");
    }

    // write settings to file

    file << to_string(types) << '\n';       // types
    file << to_string(size) << '\n';        // size
    file << to_string(count) << '\n';       // count
    file << to_string(innerRadius) << '\n'; // innerRadius
    file << to_string(resistance) << '\n';  // resistance
    file << to_string(step) << '\n';        // step

    for (int i = 0; i < types; i++) {           // attractions
        file << to_string(attractions[i][0]);
        for (int j = 1; j < types; j++)
            file << ',' << to_string(attractions[i][j]);
        file << '\n';
    }

    file << "-1" << '\n';                       // seed (-1 for preloaded)

    for (const Particle& p : particles)         // particles
        file << to_string(p.type)  << ',' <<
                to_string(p.pos.x) << ',' << to_string(p.pos.y) << ',' <<
                to_string(p.vel.x) << ',' << to_string(p.vel.y) << '\n';
    
    file.close();
}


void ParticleLife::randomisePositions()
{
    int ct = 0;
    while (ct < count) {
        for (int t = 0; t < types; t++) {
            for (int j = 0; j < settings.typeRatio[t] && ct < count; j++) {
                particles[ct++] = Particle(
                    t,
                    Vector2 {
                        GetRandomValue(0, static_cast<int>(bounds)) + GetRandomValue(0, 1000) / 1000.0f,
                        GetRandomValue(0, static_cast<int>(bounds)) + GetRandomValue(0, 1000) / 1000.0f
                    },
                    Vector2 { 0, 0 }
                );
            }
        }
    }
}

vector<int> ParticleLife::countTypes() const
{
    vector<int> typeCounts(types, 0);
    for (const Particle& p : particles)
        typeCounts[p.type]++;
    return typeCounts;
}

void ParticleLife::getComparisonData(Settings& settings, vector<Particle>& particles, vector<Particle> ghosts, long long unsigned int* frameCount) const
{
    settings = this->settings;
    particles = this->particles;
    *frameCount = this->frameCount;
}

ostream& operator << (ostream& os, const ParticleLife& particleLife)
{
    string initiation;
    if (particleLife.settings.seed == -1) initiation = "preloaded";
    else if (particleLife.settings.seed == 0) initiation = "pseudo random";
    else initiation = "seeded random";
    
    os << particleLife.settings.name << " : " <<
          initiation << endl <<
          "| types : " << particleLife.types <<
          "| size : " << particleLife.size <<
          "| count : " << particleLife.count <<
          "| innerRadius : " << particleLife.innerRadius <<
          "| resistance : " << particleLife.resistance <<
          "| step : " << particleLife.step << endl;
    
    os << "| attractions : " << endl;
    for (int i = 0; i < particleLife.types; i++) {
        os << "| | " << particleLife.attractions[i][0];
        for (int j = 1; j < particleLife.types; j++)
            os << ", " << particleLife.attractions[i][j];
        os << endl;
    }

    vector<int> typeCounts = particleLife.countTypes();
    os << "| particles : " << particleLife.particles.size() << "(" << typeCounts[0];
    for (int i = 1; i < particleLife.types; i++)
        os << ":" << typeCounts[i];
    os << ")" << endl;
    if (particleLife.particles.size() < 128)
        for (const Particle& p : particleLife.particles)
            os << "| | " << p << endl;
    else os << "| | lots..." << endl;

    return os;
}


void ParticleLife::particleInteraction(Particle& p1, Particle& p2)
{
    // calculate the square distance between the particles
    const float dx = p2.pos.x - p1.pos.x;
    const float dy = p2.pos.y - p1.pos.y;
    const float sqDist = dx*dx + dy*dy;

    // if square distance is less than 2^2 then interact
    if (sqDist <= 4.0f) {
        const float dist = sqrtf(sqDist);

        // if particles are within inner radius then repel, otherwise attract
        const float coef = (dist <= innerRadius)
            ? 1.0f - innerRadius / dist
            : attractions[p1.type][p2.type] * (dist - innerRadius) / 2.0f;

        // apply interaction force to particle velocity
        p1.vel.x += coef * (dx / dist);
        p1.vel.y += coef * (dy / dist);
    }
}
