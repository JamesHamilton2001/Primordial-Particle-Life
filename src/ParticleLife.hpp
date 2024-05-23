#ifndef PARTICLELIFE_HPP
#define PARTICLELIFE_HPP

#include "Particle.hpp"
#include "SpatialHash.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <vector>
#include <iostream>
#include <random>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;


#define PARTICLE_LIFE_MIN_NAME_LENGTH     (1)
#define PARTICLE_LIFE_MAX_NAME_LENGTH     (64)
#define PARTICLE_LIFE_MIN_FLOATING_POINT  (0.000001f)
#define PARTICLE_LIFE_MAX_FLOATING_POINT  (0.999999f)
#define PARTICLE_LIFE_MIN_TYPES           (3)
#define PARTICLE_LIFE_MAX_TYPES           (9)
#define PARTICLE_LIFE_MIN_GRID_SIZE       (3)
#define PARTICLE_LIFE_MAX_GRID_SIZE       (512)
#define PARTICLE_LIFE_MIN_COUNT           (1)
#define PARTICLE_LIFE_MAX_COUNT           (65536)
#define PARTICLE_LIFE_MIN_INNER_RADIUS    (PARTICLE_LIFE_MIN_FLOATING_POINT)
#define PARTICLE_LIFE_MAX_INNER_RADIUS    (1.0f)
#define PARTICLE_LIFE_MIN_RESISTANCE      (PARTICLE_LIFE_MIN_FLOATING_POINT)
#define PARTICLE_LIFE_MAX_RESISTANCE      (1.0f)
#define PARTICLE_LIFE_MIN_STEP            (PARTICLE_LIFE_MIN_FLOATING_POINT)
#define PARTICLE_LIFE_MAX_STEP            (PARTICLE_LIFE_MAX_FLOATING_POINT)
#define PARTICLE_LIFE_MIN_ATTRACTION      (-PARTICLE_LIFE_MAX_FLOATING_POINT)
#define PARTICLE_LIFE_MAX_ATTRACTION      (PARTICLE_LIFE_MAX_FLOATING_POINT)
#define PARTICLE_LIFE_MIN_SEED            (0)
#define PARTICLE_LIFE_MAX_SEED            (INT_MAX)
#define PARTICLE_LIFE_MIN_RATIO           (1)
#define PARTICLE_LIFE_MAX_RATIO           (PARTICLE_LIFE_MAX_COUNT)



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
        std::vector<int> typeRatio;
        std::vector<Particle> particles;
        
        Settings();
        Settings(const std::filesystem::directory_entry& dirEntry);

        friend std::ostream& operator <<(std::ostream& os, const Settings& settings);
    };

    ParticleLife() = default;
    ParticleLife(Settings& settings);
    ~ParticleLife();
    ParticleLife& operator =(const ParticleLife& other) = default;

    std::string name;
    int types;
    int size;
    float bounds;


    void update();
    void draw(unsigned int pTexID) const;

    void saveConfig() const;

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