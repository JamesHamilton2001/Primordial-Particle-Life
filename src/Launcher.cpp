#include "Launcher.hpp"

#include <raylib.h>
#include <iostream>
#include <string>

#include <filesystem>
namespace fs = std::filesystem;

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


Launcher::Launcher() :
    choice(customisedSettings)
{
    for (const auto& dirEntry : fs::directory_iterator("settings/default/")) {
        try {
            defaultSettings.push_back(ParticleLife::Settings(dirEntry));
        } catch (std::exception& e) { 
            std::cout << "Failed to load settings from " << dirEntry.path() << ": " << e.what() << std::endl;
        }
    } for (const auto& dirEntry : fs::directory_iterator("settings/custom/")) {
        try {
            customSettings.push_back(ParticleLife::Settings(dirEntry));
        } catch (std::exception& e) { 
            std::cout << "Failed to load settings from " << dirEntry.path() << ": " << e.what() << std::endl;
        }
    }

    customisedSettings.name = "";
    customisedSettings.types = 9;
    customisedSettings.size = 0;
    customisedSettings.count = 0;
    customisedSettings.innerRadius = 0;
    customisedSettings.resistance = 0;
    customisedSettings.step = 0;
    customisedSettings.attractions = std::vector<std::vector<float>>(9, std::vector<float>(9, 0));
    customisedSettings.seed = 0;
    customisedSettings.typeRatio = std::vector<int>(9, 0);
    customisedSettings.particles = std::vector<Particle>();

    std::string str;

    // TOP ROW

    tglCustom = Toggle { "Customise", false };

    // CUSTOM

    tbxName = TextBox { "", false };
    tbxTypes = TextBox { "", false };
    tbxSize = TextBox { "", false };
    tbxCount = TextBox { "", false };
    tbxInnerRadius = TextBox { "", false };
    tbxResistance = TextBox { "", false };
    tbxStep = TextBox { "", false };
    tbxAttractions = std::vector<std::vector<TextBox>>(9, std::vector<TextBox>(9, TextBox { "", false }));
    tbxTypeRatio = std::vector<TextBox>(9, TextBox { "", false });

    // PREDEFINED

    tglDefaultsCustoms = Toggle { "Defaults", false };

    str = "";
    if (!defaultSettings.empty())
        str = defaultSettings[0].name;
    if (defaultSettings.size() > 1)
        for (unsigned int i = 1; i < defaultSettings.size(); i++)
            str += ";" + defaultSettings[i].name;
    lsvDefaults = ListView { str.c_str(), 0, 0 };

    str = "";
    if (!customSettings.empty())
        str = customSettings[0].name;
    if (customSettings.size() > 1)
        for (unsigned int i = 1; i < customSettings.size(); i++)
            str += ";" + customSettings[i].name;
    lsvCustoms = ListView { str.c_str(), 0, 0 };

    btnValidateCustomInput = Button { "Validate", false };

    // BOTTOM ROW

    btnExecute = Button { "Execute", false };

}

bool Launcher::run()
{
    BeginDrawing();
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        float W = GetScreenWidth();
        float H = GetScreenHeight();
        float U = 16; // widget unit
        float M = 8;  // margin unit
        int T = customisedSettings.types;
        float lblW = 5*U;
        float tbxW = 3*U;
        Rectangle r;

        // HEADER ------------------------------------------------------------------

        Rectangle row1 = Rectangle { M, M, W-2*M, U+2*M };

        GuiGroupBox(row1, "App Settings");

        r = Rectangle { row1.x+M, row1.y+M, lblW, U };
        toggle(r, tglCustom);
        tglCustom.text = (tglCustom.active) ? "Customised" : "Predefined";

        // BODY --------------------------------------------------------------------

        // CUSTOM SETTINGS
        if (tglCustom.active) {

            float colH = std::max(7*U + 8*M, (T+3)*U + (3.5f+T)*M);
            Rectangle col1 = Rectangle {    // all 7 single settings
                M,
                row1.y + row1.height + M,
                2*M + lblW + tbxW,
                colH
            };
            Rectangle col2 = Rectangle {    // attraction matrix
                col1.x + col1.width + M,
                col1.y,
                T*tbxW + (T+1)*M,
                colH
            };
            Rectangle botm = Rectangle {
                col1.x,
                col1.y + col1.height - 1,
                col1.x + col1.width + col2.width,
                U + M 
            };

            std::cout << "col1 top left: (" << col1.x << ", " << col1.y << ")" << std::endl;
            std::cout << "col1 top right: (" << col1.x + col1.width << ", " << col1.y << ")" << std::endl;
            std::cout << "col1 bottom left: (" << col1.x << ", " << col1.y + col1.height << ")" << std::endl;
            std::cout << "col1 bottom right: (" << col1.x + col1.width << ", " << col1.y + col1.height << ")" << std::endl;

            std::cout << "col2 top left: (" << col2.x << ", " << col2.y << ")" << std::endl;
            std::cout << "col2 top right: (" << col2.x + col2.width << ", " << col2.y << ")" << std::endl;
            std::cout << "col2 bottom left: (" << col2.x << ", " << col2.y + col2.height << ")" << std::endl;
            std::cout << "col2 bottom right: (" << col2.x + col2.width << ", " << col2.y + col2.height << ")" << std::endl;

            std::cout << "botm top left: (" << botm.x << ", " << botm.y << ")" << std::endl;
            std::cout << "botm top right: (" << botm.x + botm.width << ", " << botm.y << ")" << std::endl;
            std::cout << "botm bottom left: (" << botm.x << ", " << botm.y + botm.height << ")" << std::endl;
            std::cout << "botm bottom right: (" << botm.x + botm.width << ", " << botm.y + botm.height << ")" << std::endl;
            
            // group box(es)
            r = Rectangle { col1.x, col1.y, col1.width+col2.width+M, col1.height };
            GuiGroupBox(r, "Custom Settings");
            GuiGroupBox(col2, "");
            GuiGroupBox(botm, "");

            // single settings
            r = Rectangle { col1.x+M, col1.y+M, lblW, U };          // labels...
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);
            GuiLabel(r, "Name:");         r.y += M+U;
            GuiLabel(r, "Types:");        r.y += M+U;
            GuiLabel(r, "Size:");         r.y += M+U;
            GuiLabel(r, "Count:");        r.y += M+U;
            GuiLabel(r, "Inner Radius:"); r.y += M+U;
            GuiLabel(r, "Resistance:");   r.y += M+U;
            GuiLabel(r, "Step:");         r.y += M+U;
            r = Rectangle { r.x + lblW+M, col1.y+M, tbxW, U };      // ...and text boxes
            textBox(r, tbxName);        r.y += M+U;
            textBox(r, tbxTypes);       r.y += M+U;
            textBox(r, tbxSize);        r.y += M+U;
            textBox(r, tbxCount);       r.y += M+U;
            textBox(r, tbxInnerRadius); r.y += M+U;
            textBox(r, tbxResistance);  r.y += M+U;
            textBox(r, tbxStep);        r.y += M+U;

            // attraction matrix
            r = Rectangle { col2.x, col2.y+M, col2.width, U };
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            GuiLabel(r, "Attraction Matrix:");
            r.x += M;
            r.y += M/2 + r.height;
            r.width = tbxW;
            for (int i = 0; i < T; i++) {
                for (int j = 0; j < T; j++) {
                    textBox(r, tbxAttractions[i][j]);
                    r.x += tbxW + M;
                }
                r.x = col2.x + M;
                r.y += U + M;
            }

            // type ratios
            r = Rectangle { col2.x, col2.y + U+2*M + customisedSettings.types*(U+M), col2.width, U };
            GuiLabel(r, "Type Ratios:");
            r = Rectangle { r.x + M, r.y + U+M/2, tbxW, U };
            for (int i = 0; i < T; i++) {
                textBox(r, tbxTypeRatio[i]);
                r.x += tbxW + M;
            }

            // user settings choice is custom
            choice = customisedSettings;
            
        }

        // PREDEFINED SETTINGS
        else {

            float lsvH = 6*U;
            float lsvW = 8*U;
            
            r = Rectangle { 40, 40, lsvW, U };
            toggle(r, tglDefaultsCustoms);
            tglDefaultsCustoms.text = (tglDefaultsCustoms.active) ? "Custom" : "Default";

            r.y += r.height + M;
            r.height = lsvH;

            if (tglDefaultsCustoms.active) {
                int i = lsvCustoms.activeIdx;
                listView(r, lsvCustoms);
                if (lsvCustoms.activeIdx < 0) lsvCustoms.activeIdx = i;
                choice = customSettings[lsvCustoms.activeIdx];
            }
            else {
                int i = lsvDefaults.activeIdx;
                listView(r, lsvDefaults);
                if (lsvDefaults.activeIdx < 0) lsvDefaults.activeIdx = i;
                choice = defaultSettings[lsvDefaults.activeIdx];
            }

            tbxName.text = choice.name;
            tbxTypes.text = std::to_string(choice.types);
            tbxSize.text = std::to_string(choice.size);
            tbxCount.text = std::to_string(choice.count);
            tbxInnerRadius.text = std::to_string(choice.innerRadius);
            tbxResistance.text = std::to_string(choice.resistance);
            tbxStep.text = std::to_string(choice.step);
            for (int i = 0; i < T; i++) {
                tbxTypeRatio[i].text = std::to_string(choice.typeRatio[i]);
                for (int j = 0; j < T; j++)
                    tbxAttractions[i][j].text = std::to_string(choice.attractions[i][j]);
            }
        }

        // FOOTER -----------------------------------------------------------------

        Rectangle r1 = { W/2-U, H-1.5f*U-M, 3*U, U };
        button(r1, btnExecute);

    EndDrawing();

    if (btnExecute.active) {
        return false;
    }
    else {


        return true;
    }
        

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

bool Launcher::listView(Rectangle& rect, ListView& lsv)
{
    return GuiListView(rect, lsv.text.c_str(), &lsv.scrollIdx, &lsv.activeIdx);
}

bool Launcher::strIsInt(const std::string& str)
{
    for (char c : str)
        if (!isdigit(c))
            return false;
    return true;
}

bool Launcher::strIsFloat(const std::string& str)
{
    if (str.empty()) return false;
    bool hasDot = false;
    for (unsigned int i = 0; i < str.size(); i++) {
        if (!isdigit(str[i])) {
            if (str[i] == '.' && !hasDot) {
                hasDot = true;
                continue;
            }
            return false;
        }
    }
    return true;
}

bool Launcher::validateCustomInput()
{
    return false;
    // ...
    // return true;
}