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

    // currently does not handle special characters!
    class JsonParser
    {
      public:

        JsonParser(string filePath, Settings settings);
        ~JsonParser();

        Settings& settings;
        ifstream file;
        vector<string> lines;
        char prev;
        char curr;
        unsigned long long int row;
        unsigned long long int col;
        unsigned long long int len;
        unsigned long long int count;

//         unordered_map<string, void(JsonParser::*)()> parsers = {
//             { "name", &JsonParser::parseIntoName },
//             { "types", &JsonParser::parseIntoTypes },
//             { "size", &JsonParser::parseIntoSize },
//             { "count", &JsonParser::parseIntoCount },
//             { "innerRadius", &JsonParser::parseIntoInnerRadius },
//             { "resistance", &JsonParser::parseIntoResistance },
//             { "step", &JsonParser::parseIntoStep },
//             { "attractions", &JsonParser::parseIntoAttractions },
//             { "seed", &JsonParser::parseIntoSeed },
//             { "typeRatio", &JsonParser::parseIntoTypeRatio },
//             { "particles", &JsonParser::parseIntoParticles },
//         };

        void parseIntoSettings();

        bool step();

        string parseGetKey();
        string parseGetString();
        string parseGetString(bool isKey);
        int parseGetInt();
        float parseGetFloat();

//         void parseIntoName();
//         void parseIntoTypes();
//         void parseIntoSize();
//         void parseIntoCount();
//         void parseIntoInnerRadius();
//         void parseIntoResistance();
//         void parseIntoStep();
//         void parseIntoAttractions();
//         void parseIntoSeed();
//         void parseIntoTypeRatio();
//         void parseIntoParticles();

        friend ostream& operator <<(ostream& os, const JsonParser& parser);

        string posString(int r, int c);
        string posString();
    };
};


# endif