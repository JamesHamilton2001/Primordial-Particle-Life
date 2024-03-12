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

    struct DropDownBox : public Button {
        int index;      
    };

    ParticleLife::Settings& choice;
    ParticleLife::Settings customSetting;
    std::vector<ParticleLife::Settings> defaultSettings;

    Button btnExecute;
    Toggle tglCustom;

    TextBox tbxTypes;
    TextBox tbxSize;
    TextBox tbxCount;
    TextBox tbxInnerRadius;
    TextBox tbxResistance;
    TextBox tbxStep;

    std::vector<std::vector<TextBox>> tbxAttractions;

    DropDownBox drdDefefaults;


    bool button(Button& btn, Rectangle& rect);
    bool toggle(Toggle& tgl, Rectangle& rect);
    bool textBox(TextBox& tbx, Rectangle& rect);
    bool dropDownBox(DropDownBox& drd, Rectangle& rect);

    // void scaleWidgets();

};



#endif