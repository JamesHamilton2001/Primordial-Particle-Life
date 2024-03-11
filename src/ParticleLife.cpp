#include "ParticleLife.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
namespace fs = std::filesystem;


ParticleLife::ParticleLife(Settings& settings) :
    name        (settings.name),
    types       (settings.types),
    size        (settings.size),
    bounds      (2.0f * settings.size),
    count       (settings.count),
    resistance  (settings.resistance),
    innerRadius (settings.innerRadius),
    step        (settings.step),
    attractions (settings.attractions),
    seed        (settings.seed),
    spatialHash (settings.size, settings.types)
{
    posDistr = std::uniform_real_distribution<float>(0.0f, bounds);
    gen.seed(settings.seed);
    particles.resize(count);

    if (!settings.particles.empty())
        particles = settings.particles;
    else
        randomisePositions();
}

ParticleLife::~ParticleLife()
{}


void ParticleLife::update()
{
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


void ParticleLife::saveConfig() const
{
    // open and clear temp config file
    std::ofstream file("settings/custom/temp.txt", std::ofstream::out);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open temp config file");
    }

    // write settings to file

    file << std::to_string(types) << '\n';       // types
    file << std::to_string(size) << '\n';        // size
    file << std::to_string(count) << '\n';       // count
    file << std::to_string(innerRadius) << '\n'; // innerRadius
    file << std::to_string(resistance) << '\n';  // resistance
    file << std::to_string(step) << '\n';        // step

    for (int i = 0; i < types; i++) {           // attractions
        file << std::to_string(attractions[i][0]);
        for (int j = 1; j < types; j++)
            file << ',' << std::to_string(attractions[i][j]);
        file << '\n';
    }

    file << "-1" << '\n';                       // seed (-1 for preloaded)

    for (const Particle& p : particles)         // particles
        file << std::to_string(p.type)  << ',' <<
                std::to_string(p.pos.x) << ',' << std::to_string(p.pos.y) << ',' <<
                std::to_string(p.vel.x) << ',' << std::to_string(p.vel.y) << '\n';
}


void ParticleLife::randomisePositions()
{
    for (Particle& p : particles) {
        p.type = GetRandomValue(0, types-1);
        p.pos = Vector2 { posDistr(gen), posDistr(gen) };
        p.vel = Vector2 { 0.0f, 0.0f };
    }
}

std::vector<int> ParticleLife::countTypes() const
{
    std::vector<int> typeCounts(types, 0);
    for (const Particle& p : particles)
        typeCounts[p.type]++;
    return typeCounts;
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
    std::string initiation;
    if (particleLife.seed == -1) initiation = "preloaded";
    else if (particleLife.seed == 0) initiation = "pseudo random";
    else initiation = "seeded random";
    
    os << particleLife.name << " : " <<
          initiation << std::endl <<
          "| types : " << particleLife.types << std::endl <<
          "| size : " << particleLife.size << std::endl <<
          "| count : " << particleLife.count << std::endl <<
          "| innerRadius : " << particleLife.innerRadius << std::endl <<
          "| resistance : " << particleLife.resistance << std::endl <<
          "| step : " << particleLife.step << std::endl;
    
    os << "| attractions : " << std::endl;
    for (int i = 0; i < particleLife.types; i++) {
        os << "| | " << particleLife.attractions[i][0];
        for (int j = 1; j < particleLife.types; j++)
            os << ", " << particleLife.attractions[i][j];
        os << std::endl;
    }

    std::vector<int> typeCounts = particleLife.countTypes();
    os << "| particles : " << particleLife.particles.size() << "(" << typeCounts[0];
    for (int i = 1; i < particleLife.types; i++)
        os << ":" << typeCounts[i];
    os << ")" << std::endl;
    if (particleLife.particles.size() < 128)
        for (const Particle& p : particleLife.particles)
            os << "| | " << p << std::endl;
    else os << "| | lots..." << std::endl;

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

void ParticleLife::debugGrid()
{

}

ParticleLife::Settings::Settings(const std::filesystem::directory_entry& dirEntry)
{
    std::ifstream file(dirEntry.path());
    std::string line;
    int row = 1;

    const auto clean = [](std::string& s) {
        s.erase(std::remove_if(s.begin(), s.end(), [](char c) { return std::isspace(c); }), s.end());
    };
    const auto throwBadRead = [&row](const std::string& dataPoint) {
        throw std::invalid_argument("line:" + std::to_string(row) + " Unreadable " + dataPoint + " data");
    };

    // get name from filename
    std::string fileName = dirEntry.path().filename().string();
    name = fileName.substr(0, fileName.length() - 4);

    // get types
    if (!std::getline(file, line)) throwBadRead("types");
    clean(line);
    types = std::stoi(line);
    row++;

    // get size
    if (!std::getline(file, line)) throwBadRead("size");
    clean(line);
    size = std::stoi(line);
    row++;

    //get count
    if (!std::getline(file, line)) throwBadRead("count");
    clean(line);
    count = std::stoi(line);
    row++;

    // get innerRadius
    if (!std::getline(file, line)) throwBadRead("innerRadius");
    clean(line);
    innerRadius = std::stof(line);
    row++;

    // get resistance
    if (!std::getline(file, line)) throwBadRead("resistance");
    clean(line);
    resistance = std::stof(line);
    row++;

    // get step
    if (!std::getline(file, line)) throwBadRead("step");
    clean(line);
    step = std::stof(line);
    row++;

    // get attractions
    attractions = std::vector<std::vector<float>>(types, std::vector<float>(types, 0));
    for (int i = 0; i < types; i++) {
        if (!std::getline(file, line)) throwBadRead("attractions");
        clean(line);
        std::stringstream ss(line);
        std::string str;
        for (int j = 0; j < types; j++) {
            if (!std::getline(ss, str, ',')) throwBadRead("attractions");
            clean(str);
            attractions[i][j] = std::stof(str);
        }
        row++;
    }

    // get seed
    if (!std::getline(file, line)) throwBadRead("seed");
    clean(line);
    seed = std::stoi(line);
    row++;

    // if seed is -1 then get particles
    if (seed == -1) {
        for (int i = 0; i < count; i++) {
            if (!std::getline(file, line)) throwBadRead("particle");
            clean(line);
            std::stringstream ss(line);
            std::string str;
            Particle p;
            if (!std::getline(ss, str, ',')) throwBadRead("particle type");
            clean(str);
            p.type = std::stoi(str);
            if (!std::getline(ss, str, ',')) throwBadRead("particle pos.x");
            clean(str);
            p.pos.x = std::stof(str);
            if (!std::getline(ss, str, ',')) throwBadRead("particle pos.y");
            clean(str);
            p.pos.y = std::stof(str);
            if (!std::getline(ss, str, ',')) throwBadRead("particle vel.x");
            clean(str);
            p.vel.x = std::stof(str);
            if (!std::getline(ss, str, ',')) throwBadRead("particle vel.y");
            clean(str);
            p.vel.y = std::stof(str);
            particles.emplace_back(p);
            row++;
        }
    }

    file.close();
}

