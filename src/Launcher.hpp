#ifndef LAUNCHER_HPP
#define LAUNCHER_HPP

#include "ParticleLife.hpp"

#include <string>
#include <vector>
#include <cstring>
#include <iostream>


#define DEFAULT_WIDGET_TEXT_BUFFER_SIZE 8

#if !defined(RAYGUI_VALUEBOX_MAX_CHARS)
#define RAYGUI_VALUEBOX_MAX_CHARS  32
#endif

// Custom input box that works with float values. Basicly GuiValueBox(), but with some changes
int GuiFloatBox(Rectangle bounds, const char* text, float* value, int minValue, int maxValue, bool editMode);


class Wdgt
{
  public :

    bool active;
    int textBufferSize;
    int textSize;
    char* text;

    Wdgt(int textBufferSize, const char* cstr) :
      active(false),
      textBufferSize(textBufferSize),
      textSize(strlen(cstr)),
      text(new char[textBufferSize+1]) {
        setText(cstr);
    }
    Wdgt(int textBufferSize) : Wdgt(textBufferSize, "") {};
    Wdgt(const char* cstr)   : Wdgt(DEFAULT_WIDGET_TEXT_BUFFER_SIZE, cstr) {};
    Wdgt()                   : Wdgt(DEFAULT_WIDGET_TEXT_BUFFER_SIZE) {};
    ~Wdgt() { delete[] text; }

    virtual bool update(Rectangle& rect) = 0;

    void setText(const char* cstr);

    friend std::ostream& operator <<(std::ostream& os, const Wdgt& w);

};

class Lbl : public Wdgt
{
  public: using Wdgt::Wdgt;

    bool update(Rectangle& rect) override;
};

class Btn : public Wdgt
{
  public: using Wdgt::Wdgt;
  
    bool update(Rectangle& rect) override;
};

class Tbx : public Wdgt
{
  public: using Wdgt::Wdgt;

    bool update(Rectangle& rect) override;
};

class Fbx : public Wdgt
{
  public:

    float value;
    int minValue;
    int maxValue;

    Fbx(int minValue, int maxValue, float value) {
        this->minValue = minValue;
        this->maxValue = maxValue;
        this->value = value;
    }
    Fbx(int minValue, int maxValue) : Fbx(minValue, maxValue, 0.0f) {};

    bool update(Rectangle& rect) override;

    friend std::ostream& operator <<(std::ostream& os, const Fbx& fbx);

};



class Launcher
{
  public:
    
        Launcher();
        ~Launcher() = default;
    
        bool run();

        ParticleLife::Settings& getSettings();

        Lbl lblKek = Lbl("this is kek");
        Btn btnKek = Btn("press 4 kek");
        Tbx tbxKek = Tbx("enter 4 kek");
        Fbx fbxKek = Fbx(-1, 1, -0.42f);

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

    const char defaultSettingsPath[20] = "./settings/default/";
    const char customSettingsPath[19] = "./settings/custom/";

    bool button(Rectangle& rect, Button& btn);
    bool toggle(Rectangle& rect, Toggle& tgl);
    bool textBox(Rectangle& rect, TextBox& tbx);
    bool listView(Rectangle& rect, ListView& lsv);

    bool strIsInt(const std::string& str);
    bool strIsFloat(const std::string& str);
    bool validateCustomInput();

};



#endif