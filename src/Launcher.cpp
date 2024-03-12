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
        drdDefefaults = DropDownBox { str.c_str(), 0, false };

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
    Rectangle R, r; // group and widget rect buffers
    float x, y, w, h;

    BeginDrawing();
    ClearBackground(BLACK);

        // TOP ROW

            // r = { W/4-1.5f*B, M, 4*B, B };
            // toggle(tglCustom, r);

            // r.x = 3*W/4-1.5f*B;
            // dropDownBox(drdDefefaults, r);

        // CUSTOM SETTINGS

            int t = customSetting.types;
            R = Rectangle {
                M,
                M,
                (7.5f*B + 2*M) + (t*2.5f*B + (t+2)*M),
                (6*B + 7*M),
            };
            h = t*B + M*(t+1) + 2*M + B;
            if (R.height < h) R.height = h;

            GuiGroupBox(R, "Custom Settings");

            r = Rectangle { R.x, R.y+M, 5*B, B };
            x = r.x;
            y = r.y;
            w = r.width;
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);
            GuiLabel(r, "Types:");        r.y += r.height + M;
            GuiLabel(r, "Size:");         r.y += r.height + M;
            GuiLabel(r, "Count:");        r.y += r.height + M;
            GuiLabel(r, "Inner Radius:"); r.y += r.height + M;
            GuiLabel(r, "Resistance:");   r.y += r.height + M;
            GuiLabel(r, "Step:");         r.y += r.height + M;
            r.x += r.width + M;
            r.y = y;
            r.width = 2.5f*B;
            w = r.width;
            h = r.height;
            textBox(tbxTypes, r);       r.y += r.height + M;
            textBox(tbxSize, r);        r.y += r.height + M;
            textBox(tbxCount, r);       r.y += r.height + M;
            textBox(tbxInnerRadius, r); r.y += r.height + M;
            textBox(tbxResistance, r);  r.y += r.height + M;
            textBox(tbxStep, r);        r.y += r.height + M;

            r = Rectangle {
                7.5*B + 4*M,
                M,
                w*t + M*(t+1),
                R.height
            };           
            GuiGroupBox(r, "");
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            r.x += M;
            r.y += M;
            r.height = B;
            GuiLabel(r, "Attraction Matrix:");
            r = Rectangle {
                r.x,
                r.y + M,
                w,
                h,
            };
            r.y += B;

            for (int i = 0; i < customSetting.types; i++) {
                for (int j = 0; j < customSetting.types; j++) {
                    Rectangle r2 = Rectangle {
                        r.x + j*(r.width + M),
                        r.y + i*(r.height + M),
                        r.width,
                        r.height,
                    };
                    textBox(tbxAttractions[i][j], r2);
                }
            }


        // BOTTOM ROW

            r = { W/2-B, H-1.5f*B-M, 3*B, B };
            button(btnExecute, r);

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

bool Launcher::button(Button& btn, Rectangle& rect)
{
    int ret = GuiButton(rect, btn.text.c_str());
    btn.active = ret;
    return ret;
}

bool Launcher::toggle(Toggle& tgl, Rectangle& rect)
{
    return GuiToggle(rect, tgl.text.c_str(), &tgl.active);
}

bool Launcher::textBox(TextBox& tbx, Rectangle& rect)
{
    int ret = GuiTextBox(rect, &tbx.text[0], tbx.text.capacity(), tbx.active);
    if (ret) tbx.active = !tbx.active;
    return ret;
}

bool Launcher::dropDownBox(DropDownBox& ddb, Rectangle& rect)
{
    int ret = GuiDropdownBox(rect, ddb.text.c_str(), &ddb.index, ddb.active);
    if (ret) ddb.active = !ddb.active;
    return ret;
}

