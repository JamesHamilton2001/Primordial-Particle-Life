#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <string>
#include <vector>
#include <filesystem>

#include "Particle.hpp"


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
#define PARTICLELIFE_MIN_SEED            (-1)
#define PARTICLELIFE_MAX_SEED            (INT_MAX)
#define PARTICLELIFE_MIN_RATIO           (1)
#define PARTICLELIFE_MAX_RATIO           (PARTICLELIFE_MAX_COUNT)



struct Settings
{
    static const std::string defaultSettingsDir;
    static const std::string customSettingsDir;
    static const std::string statisticsDir;

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


# endif