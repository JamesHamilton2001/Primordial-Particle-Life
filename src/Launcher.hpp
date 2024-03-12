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

    struct Widget {
        Rectangle rect;
        std::string text;
    };

    struct Button : public Widget { bool active; };
    struct Toggle : public Button {};

    struct DropDownBox : public Button {
        int index;      
    };

    ParticleLife::Settings& choice;
    ParticleLife::Settings customSetting;
    std::vector<ParticleLife::Settings> defaultSettings;

    Button btnExecute;
    Toggle tglCustom;
    DropDownBox drdDefefaults;

    


    void button(Button& btn);
    void toggle(Toggle& tgl);
    void dropDownBox(DropDownBox& drd);

    void scaleWidgets();

};



#endif