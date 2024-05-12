#ifndef LAUNCHER_HPP
#define LAUNCHER_HPP

#include "ParticleLife.hpp"

#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <limits>


#define DEFAULT_WIDGET_TEXT_BUFFER_SIZE 256

// TODO: own header and source file for widgets,
//       move constructor and destructor to source file
//       move operator= to source file
//       create copy constructors and assignment operators for all widgets
//       create custom int box widget

class Wdgt
{
  public :

    bool active;
    int textBufferSize;
    int textSize;
    char* text;

    // TODO: move to source file
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

    Wdgt(const Wdgt& other) :
        active(other.active),
        textBufferSize(other.textBufferSize),
        textSize(other.textSize),
        text(new char[textBufferSize+1]) {
            std::strcpy(text, other.text);
    }

    Wdgt& operator =(const Wdgt& other) {
        if (this != &other) {
            active = other.active;
            textBufferSize = other.textBufferSize;
            textSize = other.textSize;
            std::strcpy(text, other.text);
        }
        return *this;
    }

    virtual bool update(Rectangle& rect) = 0;

    void setText(const char* cstr);

    friend std::ostream& operator <<(std::ostream& os, const Wdgt& w);

};

class Lbl : public Wdgt
{
  public: using Wdgt::Wdgt;

    bool update(Rectangle& rect) override;
};

class Pnl : public Wdgt
{
  public:
    
    Pnl() : Wdgt() {};

    bool update(Rectangle& rect) override;
};

class Grp : public Wdgt
{
  public:

    Grp(const char* text) : Wdgt(32, text) {};

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

class Ibx : public Wdgt
{
  public:

    int value;
    int minValue;
    int maxValue;

    Ibx(int value, int minValue, int maxValue) :
        value(value),
        minValue(minValue),
        maxValue(maxValue) {};
    Ibx(int minValue, int maxValue) : Ibx(0, minValue, maxValue) {};

    bool update(Rectangle& rect) override;

    friend std::ostream& operator <<(std::ostream& os, const Ibx& ibx);
        
};

class Fbx : public Tbx
{
  public:

    float value;
    float minValue;
    float maxValue;
    char* oldText;

    Fbx(float value, float minValue, float maxValue) :
        Tbx(std::to_string(value).c_str()),
        value(value),
        minValue(minValue),
        maxValue(maxValue),
        oldText(new char[textBufferSize+1]) {
            std::strcpy(oldText, text);
        };
    Fbx(float minValue, float maxValue) : Fbx(0.0f, minValue, maxValue) {};
    Fbx() : Fbx(std::numeric_limits<float>::min(), std::numeric_limits<float>::max()) {};
    ~Fbx() { delete[] oldText; }

    Fbx(const Fbx& other) :
        Tbx(other),
        value(other.value),
        minValue(other.minValue),
        maxValue(other.maxValue),
        oldText(new char[textBufferSize+1]) {
            std::strcpy(oldText, other.oldText);
        }

    Fbx& operator =(const Fbx& other) {
        if (this != &other) {
            Tbx::operator =(other);
            value = other.value;
            minValue = other.minValue;
            maxValue = other.maxValue;
            std::strcpy(oldText, other.oldText);
        }
        return *this;
    }

    bool update(Rectangle& rect) override;

    friend std::ostream& operator <<(std::ostream& os, const Fbx& fbx);
};

class Lsv : public Wdgt
{
  public:

    int scrollIdx;
    int activeIdx;

    Lsv(const char* items) :
        Wdgt(1024, items),
        scrollIdx(0),
        activeIdx(0) {};
    Lsv() : Lsv("") {};

    bool update(Rectangle& rect) override;

    friend std::ostream& operator <<(std::ostream& os, const Lsv& lsv);
};

class FLsv : public Lsv
{
  public:

    const char* dirPath;

    FLsv(const char* dirPath) :
        Lsv(),
        dirPath(dirPath)
        { updateContents(); };

    void updateContents();

};

class TglGrp : public Wdgt
{
  public:

    int activeToggle;

    TglGrp(const char* items) :
        Wdgt(items),
        activeToggle(0) {};
    
    bool update(Rectangle& rect) override;

    friend std::ostream& operator <<(std::ostream& os, const TglGrp& tglGrp);
};

/*
class Launcher
{
  public:
    
    Launcher();
    ~Launcher() = default;

    bool run();

    ParticleLife::Settings& getSettings();

    Pnl pnlKek = Pnl();
    Grp grpKek = Grp("kek");
    Lbl lblKek = Lbl("this is kek");
    Btn btnKek = Btn("press 4 kek");
    Tbx tbxKek = Tbx("enter 4 kek");
    Fbx fbxKek = Fbx(0.42f, -1.0f, 1.0f);
    Ibx ibxKek = Ibx(42, -100, 100);
    FLsv lsvKek = FLsv("./settings/default/");
    TglGrp tglgrpKek = TglGrp("kek1;kek2;kek3");

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
*/

class Launcher
{
  public:

    Launcher();
    
    bool run();

    ParticleLife::Settings& getSettings();

  private:

    // PARTICLE LIFE SETTINGS CONTAINERS

    std::vector<ParticleLife::Settings> defaultSettings;
    std::vector<ParticleLife::Settings> customSettings;

    ParticleLife::Settings userCustomisedSettings;

    // HEADER WIDGETS

    Grp grpHeader;

    TglGrp tglgrpSettingsTab;

    // PRELOADED SETTINGS TAB WIDGETS

    Grp grpPreloadedSettings;

    TglGrp tglgrpPreloadedSettings;
    FLsv flsvDefaultSettings;
    FLsv flsvCustomSettings;

    // CUSTOMISED SETTINGS TAB WIDGETS

    Grp grpCustomisedSettings;

    Lbl lblName;        Tbx tbxName;
    Lbl lblTypes;       Ibx ibxTypes;
    Lbl lblSize;        Ibx ibxSize;
    Lbl lblCount;       Ibx ibxCount;
    Lbl lblInnerRadius; Fbx fbxInnerRadius;
    Lbl lblResistance;  Fbx fbxResistance;
    Lbl lblStep;        Fbx fbxStep;

    Lbl lblAttractions; std::vector<std::vector<Fbx>> fbxAttractions;
    Lbl lblTypeRatios;  std::vector<Ibx> fbxTypeRatios;

    // METRICS

    float W;            // window width
    float H;            // window height
    const float U;      // widget unit of measure
    const float M;      // margin unit of measure

    Rectangle headerRect;
    Rectangle bodyRect;

    // GUI HANDLER METHODS

    void header();      // handles header widgets
    bool preloaded();   // handles preloaded tab widgets, returns true on click execute preloaded settings, otherwise false
    bool customised();  // handles customise tab widgets, returns true execution of valid custom settings, otherwise false

    // UTIL METHODS

    void readPreloadedDefaultSettings();    // loads default settings from file, updates widget
    void readPreloadedCustomSettings();     // loads custom settings from file, updates widget
    void readPreloadedSettings(FLsv& flsv, std::vector<ParticleLife::Settings>& settings);

};



#endif