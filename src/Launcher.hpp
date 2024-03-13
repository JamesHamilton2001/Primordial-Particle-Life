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
        std::string text;
    };

    struct Button : public Widget { bool active; };
    struct Toggle : public Button {};
    struct TextBox : public Button {};

    // struct DropDownBox : public Button {
    //     int index;      
    // };

    struct ListView : public Widget { int scrollIdx, activeIdx; };

    ParticleLife::Settings& choice;
    ParticleLife::Settings customisedSettings;
    std::vector<ParticleLife::Settings> customSettings;
    std::vector<ParticleLife::Settings> defaultSettings;
    Toggle tglCustom;

    TextBox tbxTypes;
    TextBox tbxSize;
    TextBox tbxCount;
    TextBox tbxInnerRadius;
    TextBox tbxResistance;
    TextBox tbxStep;
    std::vector<std::vector<TextBox>> tbxAttractions;

    // DropDownBox ddbDefefaults;
    Toggle tglDefaultsCustoms;
    ListView lsvDefaults;
    ListView lsvCustoms;

    Button btnExecute;

    bool button(Rectangle& rect, Button& btn);
    bool toggle(Rectangle& rect, Toggle& tgl);
    bool textBox(Rectangle& rect, TextBox& tbx);
    // bool dropDownBox(Rectangle& rect, DropDownBox& ddb);
    bool listView(Rectangle& rect, ListView& lsv);


};



#endif