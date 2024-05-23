#include "Settings.hpp"

#include <fstream>
#include <filesystem>
#include <algorithm>


const std::string Settings::defaultSettingsDir = "./settings/default/";
const std::string Settings::customSettingsDir = "./settings/custom/";
const std::string Settings::statisticsDir = "./statistics/";

Settings::Settings() :
    name("DEFAULT_HARDCODED_SETTINGS"),
    types(PARTICLELIFE_MIN_TYPES),
    size(PARTICLELIFE_MIN_GRID_SIZE),
    count(15),
    innerRadius(0.5f),
    resistance(0.001f),
    step(0.001f),
    attractions(std::vector<std::vector<float>>( {{  0.005,  0.002, -0.002 },
                                                  { -0.002,  0.005,  0.002 },
                                                  {  0.002, -0.002,  0.005 }} )),
    seed(-1),
    typeRatio(std::vector<int>( { 1, 1, 1 } )),
    particles(std::vector<Particle>( { Particle(0, 2, 4), Particle(1, 2, 1), Particle(2, 3, 1),
                                       Particle(0, 5, 7), Particle(1, 2, 1), Particle(2, 4, 4),
                                       Particle(0, 4, 5), Particle(1, 6, 7), Particle(2, 5, 2),
                                       Particle(0, 6, 6), Particle(1, 8, 1), Particle(2, 6, 6),
                                       Particle(0, 6, 7), Particle(1, 7, 1), Particle(2, 3, 1), } ))
{}

Settings::Settings(const std::filesystem::directory_entry& dirEntry)
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
        // TODO: calculate simplified type ratio
        typeRatio = std::vector<int>(types, 0);
        for (const Particle& p : particles) typeRatio[p.type]++;
    }
    // otherwise get particle type ratio
    else { 
        typeRatio = std::vector<int>(types, 0);
        if (!std::getline(file, line)) throwBadRead("typeRatio");
        clean(line);
        std::stringstream ss(line);
        std::string str;
        for (int i = 0; i < types; i++) {
            if (!std::getline(ss, str, ',')) throwBadRead("typeRatio");
            clean(str);
            typeRatio[i] = std::stoi(str);
        }
        particles = std::vector<Particle>(count);
    }

    file.close();
}

std::ostream& operator << (std::ostream& os, const Settings& settings)
{
    os << "| name | " << settings.name << std::endl
       << "| types | " << settings.types << std::endl
       << "| size | " << settings.size << std::endl
       << "| count | " << settings.count << std::endl
       << "| innerRadius | " << settings.innerRadius << std::endl
       << "| resistance | " << settings.resistance << std::endl
       << "| step | " << settings.step << std::endl;

    os << "| attractions | " << std::endl;
    for (int i = 0; i < settings.types; i++) {
        os << "| | " << settings.attractions[i][0];
        for (int j = 1; j < settings.types; j++)
            os << ", " << settings.attractions[i][j];
        os << std::endl;
    }
    os << "| seed | " << settings.seed << std::endl;

    os << "| typeRatio | " << settings.typeRatio[0];
    for (int i = 1; i < settings.types; i++)
        os << ", " << settings.typeRatio[i];
    std::cout << std::endl;

    os << "| particles | ";
    std::vector<int> typeCounts(settings.types, 0);
    for (const Particle& p : settings.particles) typeCounts[p.type]++;
    std::cout << typeCounts[0];
    for (int i = 1; i < settings.types; i++)
        std::cout << ", " << typeCounts[i];
    std::cout << std::endl;
    if (settings.particles.size() <= 32)
        for (const Particle& p : settings.particles)
            os << "| | " << p << std::endl;
    else os << "| | lots..." << std::endl;

    return os;
}
