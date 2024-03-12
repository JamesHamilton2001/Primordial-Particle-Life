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

    customSetting.name = "";
    customSetting.types = 9;
    customSetting.size = 0;
    customSetting.count = 0;
    customSetting.innerRadius = 0;
    customSetting.resistance = 0;
    customSetting.step = 0;
    customSetting.attractions = std::vector<std::vector<float>>(9, std::vector<float>(9, 0));
    customSetting.seed = 0;
    customSetting.typeRatio = std::vector<int>(9, 0);
    customSetting.particles = std::vector<Particle>();

    std::string str;
    const Rectangle r = { 0, 0, 0, 0, };

    // TOP ROW

        btnExecute = Button { "Execute", false };

        str = defaultSettings[0].name;
        for (int i = 1; i < defaultSettings.size(); i++)
            str += ";" + defaultSettings[i].name;
        ddbDefefaults = DropDownBox { str.c_str(), 0, false };

    // CUSTOM SETTINGS

        tbxTypes = TextBox { "", false };
        tbxSize = TextBox { "", false };
        tbxCount = TextBox { "", false };
        tbxInnerRadius = TextBox { "", false };
        tbxResistance = TextBox { "", false };
        tbxStep = TextBox { "", false };
        tbxAttractions = std::vector<std::vector<TextBox>>(9, std::vector<TextBox>(9, TextBox { "", false }));

    // BOTTOM ROW

        tglCustom = Toggle { "Customise", false };

}

bool Launcher::run()
{
    const float W = GetScreenWidth();
    const float H = GetScreenHeight();
    const float B = 16; // button unit
    const float M = 8; // margin unit

    BeginDrawing();
    ClearBackground(BLACK);

        // TOP ROW

            // r = { W/4-1.5f*B, M, 4*B, B };
            // toggle(tglCustom, r);

            // r.x = 3*W/4-1.5f*B;
            // dropDownBox(ddbDefefaults, r);

        // CUSTOM SETTINGS

            const int T = customSetting.types;
            const float lblW = 5*B;
            const float tbxW = 3*B;
            Rectangle r;

            // column setup
            Rectangle col1 = Rectangle {    // all 6 single settings
                M,
                M,
                2*M + lblW + tbxW,
                6*B + 7*M
            };
            Rectangle col2 = Rectangle {    // attraction matrix
                col1.x + col1.width + M,
                col1.y,
                T*tbxW + (T+1)*M,
                std::max(B*(T+1) + M*(T+1), col1.height)
            };
            col1.height = col2.height;
            
            // group box(es)
            r = Rectangle { col1.x, col1.y, col1.width+col2.width+M, col1.height };
            GuiGroupBox(r, "Custom Settings");
            GuiGroupBox(col2, "");

            // single settings
            r = Rectangle { col1.x+M, col1.y+M, lblW, B };          // labels...
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);
            GuiLabel(r, "Types:");        r.y += M+B;
            GuiLabel(r, "Size:");         r.y += M+B;
            GuiLabel(r, "Count:");        r.y += M+B;
            GuiLabel(r, "Inner Radius:"); r.y += M+B;
            GuiLabel(r, "Resistance:");   r.y += M+B;
            GuiLabel(r, "Step:");         r.y += M+B;
            r = Rectangle { r.x + lblW+M, col1.y+M, tbxW, B };      // ...and text boxes
            textBox(r, tbxTypes);       r.y += M+B;
            textBox(r, tbxSize);        r.y += M+B;
            textBox(r, tbxCount);       r.y += M+B;
            textBox(r, tbxInnerRadius); r.y += M+B;
            textBox(r, tbxResistance);  r.y += M+B;
            textBox(r, tbxStep);        r.y += M+B;

            // attraction matrix
            r = Rectangle { col2.x, col2.y+M/2, col2.width, B };
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            GuiLabel(r, "Attraction Matrix:");
            r.x += M;
            r.y += M/2 + r.height;
            r.width = tbxW;
            for (int i = 0; i < T; i++) {
                for (int j = 0; j < T; j++) {
                    textBox(r, tbxAttractions[i][j]);
                    r.x += tbxW + M;
                } r.x = col2.x + M;
                  r.y += B + M;
            }

        // BOTTOM ROW
            Rectangle r1 = { W/2-B, H-1.5f*B-M, 3*B, B };
            button(r1, btnExecute);

    EndDrawing();

    if (btnExecute.active){
        
        if (tglCustom.active)
            choice = customSetting;
        else
            choice = defaultSettings[ddbDefefaults.index];
        
        return false;
    }
    else return true;

}

ParticleLife::Settings& Launcher::getSettings()
{
    return choice;
}

bool Launcher::button(Rectangle& rect, Button& btn)
{
    int ret = GuiButton(rect, btn.text.c_str());
    btn.active = ret;
    return ret;
}

bool Launcher::toggle(Rectangle& rect, Toggle& tgl)
{
    return GuiToggle(rect, tgl.text.c_str(), &tgl.active);
}

bool Launcher::textBox(Rectangle& rect, TextBox& tbx)
{
    int ret = GuiTextBox(rect, &tbx.text[0], tbx.text.capacity(), tbx.active);
    if (ret) tbx.active = !tbx.active;
    return ret;
}

bool Launcher::dropDownBox(Rectangle& rect, DropDownBox& ddb)
{
    int ret = GuiDropdownBox(rect, ddb.text.c_str(), &ddb.index, ddb.active);
    if (ret) ddb.active = !ddb.active;
    return ret;
}

