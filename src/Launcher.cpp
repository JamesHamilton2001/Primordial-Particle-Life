#include "Launcher.hpp"

#include <raylib.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>
namespace fs = std::filesystem;

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"



void Wdgt::setText(const char* cstr)
{
    textSize = 0;
    while (cstr[textSize] != '\0' && textSize < textBufferSize) {
        text[textSize] = cstr[textSize];
        textSize++;
    } text[textSize] = '\0';
}

std::ostream& operator <<(std::ostream& os, const Wdgt& w)
{
    return os << w.active << ", " << w.textSize <<"/"<< w.textBufferSize << ", \"" << w.text << "\"";
}

bool Lbl::update(Rectangle& rect)
{
    return GuiLabel(rect, text);
}

bool Btn::update(Rectangle& rect)
{
    active = GuiButton(rect, &text[0]);
    return active;
}

bool Tbx::update(Rectangle& rect)
{
    if (GuiTextBox(rect, text, textBufferSize+1, active)) {
        active = !active;
        if (!active)
            textSize = strlen(text);
        return true;
    } return false;
}

bool Ibx::update(Rectangle& rect)
{
    if (GuiValueBox(rect, NULL, &value, minValue, maxValue, active)) {
        active = !active;
        return true;
    } return false;
}

std::ostream& operator <<(std::ostream& os, const Ibx& ibx)
{
    return os << ibx.minValue << " <= " << ibx.value << " <= " << ibx.maxValue;
}

bool Fbx::update(Rectangle& rect)
{
    if (GuiTextBox(rect, text, textBufferSize+1, active))
    {
        active = !active;

        if (!active) // new value entered
        {
            bool isFloat = true;

            int sign = 0;
            int pIdx = -1;

            for (int i = 0; text[i] != '\0'; i++) { // validate input, get sign value and point index
                char c = text[i];
                
                if (c >= '0' && c <= '9') { // digit valid at any position
                    continue;
                }
                else if (c == '-') {
                    if (sign != 0 || pIdx != -1) {  // -sign must be first character
                        isFloat = false;
                        break;
                    } else sign = -1;
                }
                else if (c == '+') {
                    if (sign != 0 || pIdx != -1) {  // +sign must be first character
                        isFloat = false;
                        break;
                    } else sign = 1;
                }
                else if (c == '.') { // only one point allowed
                    if (pIdx != -1) {
                        isFloat = false;
                        break;
                    } else pIdx = i;
                }
                else { // invalid character
                    isFloat = false;
                    break;
                }
            }
            
            if (isFloat) {
                float v = strtof(text, nullptr);
                if (v <= maxValue && v >= minValue) {
                    value = v;
                    strcpy(oldText, text);
                    textSize = strlen(text);
                    return true; // return true if valid in range float input
                }
            }
            strcpy(text, oldText); // revert text if invalid float input
        }
        return false; // return false if invalid float input
    }
    return false; // return false if contents not updated
}

std::ostream& operator <<(std::ostream& os, const Fbx& fbx)
{
    return os << static_cast<const Tbx&>(fbx) << ", " << fbx.value;
}


Launcher::Launcher() :
    choice(customisedSettings)
{
    for (const auto& dirEntry : fs::directory_iterator(defaultSettingsPath)) {
        try {
            defaultSettings.push_back(ParticleLife::Settings(dirEntry));
        } catch (std::exception& e) { 
            std::cout << "Failed to load settings from " << dirEntry.path() << ": " << e.what() << std::endl;
        }
    } for (const auto& dirEntry : fs::directory_iterator(customSettingsPath)) {
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

        // BEGIN TESTYNESS
        try {
            r = Rectangle { 100, 600, 600, 20 };

            lblKek.update(r);

            // test button update and text manipulation
            r.y += 25;
            if (btnKek.update(r)) {
                btnKek.setText((std::string(btnKek.text) + std::string(" kek")).c_str());
                const char* tmp = btnKek.text;
                while (*tmp != '\0') {
                    tmp++;
                }
                if (*tmp != '\0') throw std::runtime_error("not null terminated");
                std::cout << "btnKek: " << btnKek << std::endl;
            }
            // test text box update and text manipulation
            r.y += 25;
            if (tbxKek.update(r)) {
                const char* tmp = tbxKek.text;
                while (*tmp != '\0') {
                    tmp++;
                }
                if (*tmp != '\0') throw std::runtime_error("not null terminated");
                std::cout << "tbxKek" << tbxKek << std::endl;
            }
            // test int box update and text manipulation
            r.y += 25;
            if (ibxKek.update(r)) {
                std::cout << "ibxKek" << ibxKek << std::endl;
            }
            // test float box update and text manipulation
            r.y += 25;
            if (fbxKek.update(r)) {
                std::cout << "fbxKek" << fbxKek << std::endl;
            }
        }
        catch (std::exception& e) { std::cout << "Exception: " << e.what() << std::endl; }
        // FINISH TESTYNESS

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
                U + 2*M
            };

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

            // VALIDATION AND MANIPULATION

            r = Rectangle { botm.x + M, botm.y + M, 3*U, U };
            button(r, btnValidateCustomInput);

            if (btnValidateCustomInput.active) {
                if (validateCustomInput()) {
                    // ...
                }
                else {
                    // ...
                }
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
    std::cout << "Validating custom input:" << std::endl;
    std::cout << "| Name:             " << tbxName.text << std::endl;
    std::cout << "| Types:            " << tbxTypes.text << std::endl;
    std::cout << "| Size:             " << tbxSize.text << std::endl;
    std::cout << "| Count:            " << tbxCount.text << std::endl;
    std::cout << "| Inner Radius:     " << tbxInnerRadius.text << std::endl;
    std::cout << "| Resistance:       " << tbxResistance.text << std::endl;
    std::cout << "| Step:             " << tbxStep.text << std::endl;
    std::cout << "| Attraction Matrix:" << std::endl;
    for (int i = 0; i < customisedSettings.types; i++) {
        std::cout << "| |";
        for (int j = 0; j < customisedSettings.types; j++)
            std::cout << " [" << tbxAttractions[i][j].text << "]";
        std::cout << std::endl;
    }
    std::cout << "| Type Ratios:      ";
    for (int i = 0; i < customisedSettings.types; i++)
        std::cout << " [" << tbxTypeRatio[i].text << "]";
    std::cout << std::endl;

    return false;
}