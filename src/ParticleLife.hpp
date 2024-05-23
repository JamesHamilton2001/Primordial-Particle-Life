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

#define PARTICLELIFE_DEFAULT_SETTINGS_DIR "./settings/default/"
#define PARTICLELIFE_CUSTOM_SETTINGS_DIR  "./settings/custom/"

#define PARTICLELIFE_MIN_NAME_LENGTH     (1)
#define PARTICLELIFE_MAX_NAME_LENGTH     (64)
#define PARTICLELIFE_MIN_FLOATING_POINT  (0.000001f)
#define PARTICLELIFE_MAX_FLOATING_POINT  (0.999999f)
#define PARTICLELIFE_MIN_TYPES           (3)
#define PARTICLELIFE_MAX_TYPES           (9)
#define PARTICLELIFE_MIN_GRID_SIZE       (3)
#define PARTICLELIFE_MAX_GRID_SIZE       (512)
#define PARTICLELIFE_MIN_COUNT           (PARTICLELIFE_MIN_TYPES)
#define PARTICLELIFE_MAX_COUNT           (65536)
#define PARTICLELIFE_MIN_INNER_RADIUS    (PARTICLELIFE_MIN_FLOATING_POINT)
#define PARTICLELIFE_MAX_INNER_RADIUS    (1.0f)
#define PARTICLELIFE_MIN_RESISTANCE      (PARTICLELIFE_MIN_FLOATING_POINT)
#define PARTICLELIFE_MAX_RESISTANCE      (1.0f)
#define PARTICLELIFE_MIN_STEP            (PARTICLELIFE_MIN_FLOATING_POINT)
#define PARTICLELIFE_MAX_STEP            (PARTICLELIFE_MAX_FLOATING_POINT)
#define PARTICLELIFE_MIN_ATTRACTION      (-PARTICLELIFE_MAX_FLOATING_POINT)
#define PARTICLELIFE_MAX_ATTRACTION      (PARTICLELIFE_MAX_FLOATING_POINT)
#define PARTICLELIFE_MIN_SEED            (0)
#define PARTICLELIFE_MAX_SEED            (INT_MAX)
#define PARTICLELIFE_MIN_RATIO           (1)
#define PARTICLELIFE_MAX_RATIO           (PARTICLELIFE_MAX_COUNT)



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