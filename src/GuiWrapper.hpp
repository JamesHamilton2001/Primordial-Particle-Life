#ifndef GUI_WRAPPER_HPP
#define GUI_WRAPPER_HPP

#define DEFAULT_WIDGET_TEXT_BUFFER_SIZE 256

#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <limits>

#include <raylib.h>
// #include "raygui.h"


Color getGuiBackgroundColor();


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

    Grp(const char* text) : Wdgt(64, text) {};
    Grp() : Grp("") {};

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


#endif