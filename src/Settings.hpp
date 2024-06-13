#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "Particle.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <functional>

using namespace std;



constexpr int   PARTICLELIFE_MIN_NAME_LENGTH     = 1;
constexpr int   PARTICLELIFE_MAX_NAME_LENGTH     = 64;
constexpr float PARTICLELIFE_MIN_FLOATING_POINT  = 0.000001f;
constexpr float PARTICLELIFE_MAX_FLOATING_POINT  = 0.999999f;
constexpr int   PARTICLELIFE_MIN_TYPES           = 3;
constexpr int   PARTICLELIFE_MAX_TYPES           = 9;
constexpr int   PARTICLELIFE_MIN_GRID_SIZE       = 3;
constexpr int   PARTICLELIFE_MAX_GRID_SIZE       = 512;
constexpr int   PARTICLELIFE_MIN_COUNT           = PARTICLELIFE_MIN_TYPES;
constexpr int   PARTICLELIFE_MAX_COUNT           = 65536;
constexpr float PARTICLELIFE_MIN_INNER_RADIUS    = PARTICLELIFE_MIN_FLOATING_POINT;
constexpr float PARTICLELIFE_MAX_INNER_RADIUS    = 1.0f;
constexpr float PARTICLELIFE_MIN_RESISTANCE      = PARTICLELIFE_MIN_FLOATING_POINT;
constexpr float PARTICLELIFE_MAX_RESISTANCE      = 1.0f;
constexpr float PARTICLELIFE_MIN_STEP            = PARTICLELIFE_MIN_FLOATING_POINT;
constexpr float PARTICLELIFE_MAX_STEP            = PARTICLELIFE_MAX_FLOATING_POINT;
constexpr float PARTICLELIFE_MIN_ATTRACTION      = -PARTICLELIFE_MAX_FLOATING_POINT;
constexpr float PARTICLELIFE_MAX_ATTRACTION      = PARTICLELIFE_MAX_FLOATING_POINT;
constexpr int   PARTICLELIFE_MIN_SEED            = -1;
constexpr int   PARTICLELIFE_MAX_SEED            = INT_MAX;
constexpr int   PARTICLELIFE_MIN_RATIO           = 1;
constexpr int   PARTICLELIFE_MAX_RATIO           = PARTICLELIFE_MAX_COUNT;

constexpr const float PARTICLELIFE_DIAMETER = 2.0f;

constexpr const char* PARTICLELIFE_DEFAULT_SETTINGS_DIR   = "./settings/default/";
constexpr const char* PARTICLELIFE_CUSTOM_SETTINGS_DIR    = "./settings/custom/";
constexpr const char* PARTICLELIFE_STATS_DATA_DIR         = "./statistics/data/";
constexpr const char* PARTICLELIFE_STATS_SCREENSHOTS_DIR  = "./statistics/screenshots/";


/*                                                          RED  GRN  BLU  YLW  MGT  CYN  ORG  PRP  WHT */
constexpr const unsigned char R[PARTICLELIFE_MAX_TYPES] = { 255, 000, 000, 255, 255, 000, 255, 127, 255 };
constexpr const unsigned char G[PARTICLELIFE_MAX_TYPES] = { 000, 255, 000, 255, 000, 255, 127, 000, 255 };
constexpr const unsigned char B[PARTICLELIFE_MAX_TYPES] = { 000, 000, 255, 000, 255, 255, 000, 127, 255 };




struct Settings
{
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
    Settings(string filePath);

    void generateParticleData();

    void save() const;

    friend ostream& operator <<(ostream& os, const Settings& settings);

  private:

    // unordered_map<string, function<void(ifstream&, string&, unsigned long long int&)>> readers = {
    //     { "name", [this](ifstream& file, string& line, unsigned long long int& lno)         { name = getNextString(file, line, lno); } },
    //     { "types", [this](ifstream& file, string& line, unsigned long long int& lno)        { types = getNextInt(file, line, lno); } },
    //     { "size", [this](ifstream& file, string& line, unsigned long long int& lno)         { size = getNextInt(file, line, lno); } },
    //     { "count", [this](ifstream& file, string& line, unsigned long long int& lno)        { count = getNextInt(file, line, lno); } },
    //     { "innerRadius", [this](ifstream& file, string& line, unsigned long long int& lno)  { innerRadius = getNextFloat(file, line, lno); } },
    //     { "resistance", [this](ifstream& file, string& line, unsigned long long int& lno)   { resistance = getNextFloat(file, line, lno); } },
    //     { "step", [this](ifstream& file, string& line, unsigned long long int& lno)         { step = getNextFloat(file, line, lno); } },
    //     { "attractions", [this](ifstream& file, string& line, unsigned long long int& lno)  { readFloatMatrix(file, line, lno, attractions); } },
    //     { "seed", [this](ifstream& file, string& line, unsigned long long int& lno)         { seed = getNextInt(file, line, lno); } },
    //     { "typeRatio", [this](ifstream& file, string& line, unsigned long long int& lno)    { readIntVector(file, line, lno, typeRatio); } },
    //     { "particles", [this](ifstream& file, string& line, unsigned long long int& lno)    { readParticleVector(file, line, lno, particles); } },
    // };

    unordered_map<string, void (Settings::*)(ifstream&, string&, unsigned long long int&)> attributteParsers = {
        { "name", &Settings::parseName },
        { "types", &Settings::parseTypes },
        { "size", &Settings::parseSize },
        { "count", &Settings::parseCount },
        { "innerRadius", &Settings::parseInnerRadius },
        { "resistance", &Settings::parseResistance },
        { "step", &Settings::parseStep },
        { "attractions", &Settings::parseAttractions },
        { "seed", &Settings::parseSeed },
        { "typeRatio", &Settings::parseTypeRatio },
        { "particles", &Settings::parseParticles },
    };

    const string getNextString(ifstream& file, string& line, unsigned long long int& lno) const;
    const int getNextInt(ifstream& file, string& line, unsigned long long int& lno) const;
    const float getNextFloat(ifstream& file, string& line, unsigned long long int& lno) const;

    void parseName(ifstream& file, string& line, unsigned long long int& lno);
    void parseTypes(ifstream& file, string& line, unsigned long long int& lno);
    void parseSize(ifstream& file, string& line, unsigned long long int& lno);
    void parseCount(ifstream& file, string& line, unsigned long long int& lno);
    void parseInnerRadius(ifstream& file, string& line, unsigned long long int& lno);
    void parseResistance(ifstream& file, string& line, unsigned long long int& lno);
    void parseStep(ifstream& file, string& line, unsigned long long int& lno);
    void parseAttractions(ifstream& file, string& line, unsigned long long int& lno);
    void parseSeed(ifstream& file, string& line, unsigned long long int& lno);
    void parseTypeRatio(ifstream& file, string& line, unsigned long long int& lno);
    void parseParticles(ifstream& file, string& line, unsigned long long int& lno);
};


# endif