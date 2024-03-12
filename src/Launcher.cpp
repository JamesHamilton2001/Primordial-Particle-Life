#include "Launcher.hpp"

#include <raylib.h>
#include <iostream>
#include <string>

#include <filesystem>
namespace fs = std::filesystem;

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


Launcher::Launcher() :
    choice(customSetting)
{
    for (const auto& dirEntry : fs::directory_iterator("settings/default/")) {
        try {
            defaultSettings.push_back(ParticleLife::Settings(dirEntry));
        } catch (std::exception& e) { 
            std::cout << "Failed to load settings from " << dirEntry.path() << ": " << e.what() << std::endl;
        }
    }

    std::string str;
    const Rectangle r = { 0, 0, 0, 0, };

    // TOP ROW
    btnExecute = Button { r, "Execute", false };

    str = defaultSettings[0].name;
    for (int i = 1; i < defaultSettings.size(); i++)
        str += ";" + defaultSettings[i].name;
    drdDefefaults = DropDownBox { r, str.c_str(), 0, false };

    // BOTTOM ROW
    tglCustom = Toggle { r, "Customise", false };

    scaleWidgets();
}

bool Launcher::run()
{
    BeginDrawing();
    ClearBackground(LIGHTGRAY);

        // TOP ROW
        toggle(tglCustom);
        dropDownBox(drdDefefaults);

        // BOTTOM ROW
        button(btnExecute);

    EndDrawing();

    if (btnExecute.active){
        
        if (tglCustom.active)
            choice = customSetting;
        else
            choice = defaultSettings[drdDefefaults.index];
        
        return false;
    }
    else return true;

}

ParticleLife::Settings& Launcher::getSettings()
{
    return choice;
}

void Launcher::button(Button& btn)
{
    btn.active = GuiButton(btn.rect, btn.text.c_str());
}

void Launcher::toggle(Toggle& tgl)
{
    GuiToggle(tgl.rect, tgl.text.c_str(), &tgl.active);
}

void Launcher::dropDownBox(DropDownBox& ddb)
{
    if (GuiDropdownBox(ddb.rect, ddb.text.c_str(), &ddb.index, ddb.active))
        ddb.active = !ddb.active;
}

void Launcher::scaleWidgets()
{
    const float W = GetScreenWidth();   // screen width
    const float H = GetScreenHeight();  // screen height
    const float B = 32;                 // button unit
    const float M = 16;                 // margin unit

    Rectangle r;

    // TOP ROW
    tglCustom.rect = Rectangle { W/4-1.5f*B, M, 3*B, B };
    drdDefefaults.rect = Rectangle { 3*W/4-1.5f*B, M, 3*B, B };

    // BOTTOM ROW
    btnExecute.rect = Rectangle { W/2-B, H-1.5f*B-M, 3*B, B };
}