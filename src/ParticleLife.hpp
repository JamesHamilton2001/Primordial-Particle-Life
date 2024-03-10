#ifndef PARTICLELIFE_HPP
#define PARTICLELIFE_HPP

#include "Particle.hpp"
#include "SpatialHash.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <vector>
#include <random>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;


class ParticleLife
{
 public:

    struct Settings
    {
        std::string name;
        int types;
        int size;
        int count;
        float innerRadius;
        float resistance;
        float step;
        std::vector<std::vector<float>> attractions;
        int seed;
        std::vector<Particle> particles;
        
        Settings() = default;
        Settings(const std::filesystem::directory_entry& dirEntry);
    };

    ParticleLife(Settings& settings);
    ~ParticleLife();

    const std::string name;
    const int types;
    const int size;
    const float bounds;


    void update();
    void draw(unsigned int pTexID) const;

    void drawGrid() const   { spatialHash.drawGrid(); }
    void drawGhosts(unsigned int pTexID) const { spatialHash.drawGhosts(pTexID); }
    void drawSoftBorder() const;

    void randomisePositions();

    std::vector<int> countTypes() const;

    void printCell(int row, int col);
    void printCellAtPos(Vector2 pos);

    friend std::ostream& operator << (std::ostream& os, const ParticleLife& particleLife);

 private:

    int count;
    float resistance;
    float innerRadius;
    float step;
    std::vector<std::vector<float>> attractions;

    std::vector<Particle> particles;

    int seed;

    SpatialHash spatialHash;

    std::mt19937 gen;
    std::uniform_real_distribution<float> posDistr;

    void particleInteraction(Particle& p1, Particle& p2);

    void debugGrid();

};


#endif