#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "Particle.hpp"
#include <string>
#include <vector>
#include <filesystem>

using namespace std;



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
    static const string defaultSettingsDir;
    static const string customSettingsDir;
    static const string statisticsDir;

    string name;
    int types;
    int size;
    int count;
    float innerRadius;
    float resistance;
    float step;
    vector<vector<float>> attractions;
    int seed;
    vector<int> typeRatio;
    vector<Particle> particles;
    
    Settings();
    Settings(const filesystem::directory_entry& dirEntry);

    void generateParticleData();

    friend ostream& operator <<(ostream& os, const Settings& settings);
};


# endif