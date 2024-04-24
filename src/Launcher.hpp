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

    struct Widget { std::string text; };
    struct Button : public Widget { bool active; };
    struct Toggle : public Button {};
    struct TextBox : public Button {};
    struct ListView : public Widget { int scrollIdx, activeIdx; };

    ParticleLife::Settings& choice;
    ParticleLife::Settings customisedSettings;
    std::vector<ParticleLife::Settings> customSettings;
    std::vector<ParticleLife::Settings> defaultSettings;

    TextBox tbxName;
    TextBox tbxTypes;
    TextBox tbxSize;
    TextBox tbxCount;
    TextBox tbxInnerRadius;
    TextBox tbxResistance;
    TextBox tbxStep;
    std::vector<std::vector<TextBox>> tbxAttractions;
    std::vector<TextBox> tbxTypeRatio;
    Toggle tglCustom;
    Toggle tglDefaultsCustoms;
    ListView lsvDefaults;
    ListView lsvCustoms;
    Button btnValidateCustomInput;
    Button btnExecute;

    bool button(Rectangle& rect, Button& btn);
    bool toggle(Rectangle& rect, Toggle& tgl);
    bool textBox(Rectangle& rect, TextBox& tbx);
    bool listView(Rectangle& rect, ListView& lsv);

    bool strIsInt(const std::string& str);
    bool strIsFloat(const std::string& str);
    bool validateCustomInput();

};



#endif