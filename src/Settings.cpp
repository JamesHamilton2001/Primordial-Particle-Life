#include "Settings.hpp"

#include <fstream>
#include <filesystem>
#include <algorithm>


const string Settings::defaultSettingsDir = "./settings/default/";
const string Settings::customSettingsDir = "./settings/custom/";
const string Settings::statisticsDir = "./statistics/";

Settings::Settings() :
    name("DEFAULT_HARDCODED_SETTINGS"),
    types(PARTICLELIFE_MIN_TYPES),
    size(PARTICLELIFE_MIN_GRID_SIZE),
    count(15),
    innerRadius(0.5f),
    resistance(0.001f),
    step(0.001f),
    attractions(vector<vector<float>>( {{  0.005,  0.002, -0.002 },
                                        { -0.002,  0.005,  0.002 },
                                        {  0.002, -0.002,  0.005 }} )),
    seed(-1),
    typeRatio(vector<int>( { 1, 1, 1 } )),
    particles(vector<Particle>( { Particle(0, 2, 4), Particle(1, 2, 1), Particle(2, 3, 1),
                                  Particle(0, 5, 7), Particle(1, 2, 1), Particle(2, 4, 4),
                                  Particle(0, 4, 5), Particle(1, 6, 7), Particle(2, 5, 2),
                                  Particle(0, 6, 6), Particle(1, 8, 1), Particle(2, 6, 6),
                                  Particle(0, 6, 7), Particle(1, 7, 1), Particle(2, 3, 1), } ))
{}

Settings::Settings(const filesystem::directory_entry& dirEntry)
{
    ifstream file(dirEntry.path());
    string line;
    int row = 1;

    const auto clean = [](string& s) {
        s.erase(remove_if(s.begin(), s.end(), [](char c) { return isspace(c); }), s.end());
    };
    const auto throwBadRead = [&row](const string& dataPoint) {
        throw invalid_argument("line:" + to_string(row) + " Unreadable " + dataPoint + " data");
    };

    // get name from filename
    string fileName = dirEntry.path().filename().string();
    name = fileName.substr(0, fileName.length() - 4);

    // get types
    if (!getline(file, line)) throwBadRead("types");
    clean(line);
    types = stoi(line);
    row++;

    // get size
    if (!getline(file, line)) throwBadRead("size");
    clean(line);
    size = stoi(line);
    row++;

    //get count
    if (!getline(file, line)) throwBadRead("count");
    clean(line);
    count = stoi(line);
    row++;

    // get innerRadius
    if (!getline(file, line)) throwBadRead("innerRadius");
    clean(line);
    innerRadius = stof(line);
    row++;

    // get resistance
    if (!getline(file, line)) throwBadRead("resistance");
    clean(line);
    resistance = stof(line);
    row++;

    // get step
    if (!getline(file, line)) throwBadRead("step");
    clean(line);
    step = stof(line);
    row++;

    // get attractions
    attractions = vector<vector<float>>(types, vector<float>(types, 0));
    for (int i = 0; i < types; i++) {
        if (!getline(file, line)) throwBadRead("attractions");
        clean(line);
        stringstream ss(line);
        string str;
        for (int j = 0; j < types; j++) {
            if (!getline(ss, str, ',')) throwBadRead("attractions");
            clean(str);
            attractions[i][j] = stof(str);
        }
        row++;
    }

    // get seed
    if (!getline(file, line)) throwBadRead("seed");
    clean(line);
    seed = stoi(line);
    row++;

    // if seed is -1 then get particles
    if (seed == -1) {
        for (int i = 0; i < count; i++) {
            if (!getline(file, line)) throwBadRead("particle");
            clean(line);
            stringstream ss(line);
            string str;
            Particle p;
            if (!getline(ss, str, ',')) throwBadRead("particle type");
            clean(str);
            p.type = stoi(str);
            if (!getline(ss, str, ',')) throwBadRead("particle pos.x");
            clean(str);
            p.pos.x = stof(str);
            if (!getline(ss, str, ',')) throwBadRead("particle pos.y");
            clean(str);
            p.pos.y = stof(str);
            if (!getline(ss, str, ',')) throwBadRead("particle vel.x");
            clean(str);
            p.vel.x = stof(str);
            if (!getline(ss, str, ',')) throwBadRead("particle vel.y");
            clean(str);
            p.vel.y = stof(str);
            particles.emplace_back(p);
            row++;
        }
        // TODO: calculate simplified type ratio
        typeRatio = vector<int>(types, 0);
        for (const Particle& p : particles) typeRatio[p.type]++;
    }
    // otherwise get particle type ratio
    else { 
        typeRatio = vector<int>(types, 0);
        if (!getline(file, line)) throwBadRead("typeRatio");
        clean(line);
        stringstream ss(line);
        string str;
        for (int i = 0; i < types; i++) {
            if (!getline(ss, str, ',')) throwBadRead("typeRatio");
            clean(str);
            typeRatio[i] = stoi(str);
        }
        particles = vector<Particle>(count);
    }

    file.close();
}

void Settings::generateParticleData()
{
    // dont overwrite for preloading particles, recalculate ratio
    if (seed == -1) {
        typeRatio = vector<int>(types, 0);
        for (Particle& p : particles)
            typeRatio[p.type]++;
        return;
    }

    SetRandomSeed(seed);
    particles.clear();
    int ct = 0;
    while (ct < count) {
        for (int t = 0; t < types; t++) {
            for (int i = 0; i < typeRatio[t] && ct < count; i++) {
                particles.emplace_back(Particle(
                    t,
                    GetRandomValue(0, static_cast<int>(2*size)) + GetRandomValue(0, 1000) / 1000.0f,
                    GetRandomValue(0, static_cast<int>(2*size)) + GetRandomValue(0, 1000) / 1000.0f
                ));
                ct++;
            }
        }
    }    
}

ostream& operator << (ostream& os, const Settings& settings)
{
    os << "| name | " << settings.name << endl
       << "| types | " << settings.types << endl
       << "| size | " << settings.size << endl
       << "| count | " << settings.count << endl
       << "| innerRadius | " << settings.innerRadius << endl
       << "| resistance | " << settings.resistance << endl
       << "| step | " << settings.step << endl;

    os << "| attractions | " << endl;
    for (int i = 0; i < settings.types; i++) {
        os << "| | " << settings.attractions[i][0];
        for (int j = 1; j < settings.types; j++)
            os << ", " << settings.attractions[i][j];
        os << endl;
    }
    os << "| seed | " << settings.seed << endl;

    os << "| typeRatio | " << settings.typeRatio[0];
    for (int i = 1; i < settings.types; i++)
        os << ", " << settings.typeRatio[i];
    cout << endl;

    os << "| particles | ";
    vector<int> typeCounts(settings.types, 0);
    for (const Particle& p : settings.particles) typeCounts[p.type]++;
    cout << typeCounts[0];
    for (int i = 1; i < settings.types; i++)
        cout << ", " << typeCounts[i];
    cout << endl;
    if (settings.particles.size() <= 32)
        for (const Particle& p : settings.particles)
            os << "| | " << p << endl;
    else os << "| | lots..." << endl;

    return os;
}
