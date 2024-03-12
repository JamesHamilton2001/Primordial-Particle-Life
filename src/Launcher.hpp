#ifndef LAUNCHER_HPP
#define LAUNCHER_HPP

#include "ParticleLife.hpp"

#include <string>
#include <vector>


class Launcher
{
 public:
    
        Launcher();
        ~Launcher() = default;
    
        bool run();

        ParticleLife::Settings& getSettings();

 private:

    bool launch;
    bool customSetting;

    std::string defaultDropDownText;
    bool defaultDropDownActive;
    int defaultDropDownActiveIndex;
    bool defaultDropDownEditMode;

    std::vector<ParticleLife::Settings> defaultSettings;

};



#endif