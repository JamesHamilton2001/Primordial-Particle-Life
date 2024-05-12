#include "Launcher.hpp"

#include <raylib.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <dirent.h>

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

bool Pnl::update(Rectangle& rect)
{
    return GuiPanel(rect, NULL);
}

bool Grp::update(Rectangle& rect)
{
    return GuiGroupBox(rect, text);
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

bool Lsv::update(Rectangle& rect)
{
    if (GuiListView(rect, text, &scrollIdx, &activeIdx))
        return true;
    return false;
}

std::ostream& operator <<(std::ostream& os, const Lsv& lsv)
{
    return os << lsv.scrollIdx << ", " << lsv.activeIdx << ", " << lsv.text;
}

void FLsv::updateContents()
{
    struct dirent* entry;
    DIR* dir = opendir(dirPath);
    if (dir == NULL) {
        strcpy(text, "Failed to open directory.");
        return;
    }
    strcpy(text, "");
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        strcat(text, entry->d_name);
        strcat(text, ";");
    }
    closedir(dir);
    if (strlen(text) > 0)
        text[strlen(text)-1] = '\0';
}

bool TglGrp::update(Rectangle& rect)
{
    if (GuiToggleGroup(rect, text, &activeToggle))
        return true;
    return false;
}

std::ostream& operator <<(std::ostream& os, const TglGrp& tglGrp)
{
    return os << tglGrp.activeToggle << ", " << tglGrp.text;
}


/*
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


            r = Rectangle { 100, 500, 600, 20 };

            grpKek.update(r);
            lblKek.update(r);

            r.y += 25;
            if (btnKek.update(r))
                std::cout << "btnKek " << btnKek << std::endl;
            r.y += 25;
            if (tbxKek.update(r))
                std::cout << "tbxKek " << tbxKek << std::endl;
            r.y += 25;
            if (ibxKek.update(r))
                std::cout << "ibxKek " << ibxKek << std::endl;
            r.y += 25;
            if (fbxKek.update(r))
                std::cout << "fbxKek " << fbxKek << std::endl;
            r.y += 25; r.height = 100;
            if (lsvKek.update(r))
                std::cout << "flsvKek " << lsvKek << std::endl;
            r = (Rectangle) { r.x, r.y+100, 25, 20 };
            if (tglgrpKek.update(r))
                std::cout << "tglgrpKek " << tglgrpKek << std::endl;
            
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
*/

Launcher::Launcher() :

    // HEADER WIDGETS

    grpHeader("Simulation Settings"),

    tglgrpSettingsTab("Preloaded;Customised"),

    // PRELOADED SETTINGS TAB WIDGETS

    grpPreloadedSettings("Preloaded Settings"),

    tglgrpSelectPreloadedSettings("Default;Custom"),
    flsvSelectDefaultSettings("./settings/default/"),
    flsvSelectCustomSettings ("./settings/custom/"),

    // CUSTOMISED SETTINGS TAB WIDGETS

    grpCustomisedSettings("Customise Settings"),
    lblName         ("Name:"),          tbxName(),
    lblTypes        ("Types:"),         ibxTypes(0, 9),
    lblSize         ("Size:"),          ibxSize( 0, 256),
    lblCount        ("Count:"),         ibxCount(0, 65536),
    lblInnerRadius  ("Inner Radius:"),  fbxInnerRadius(0, 2.0f),
    lblResistance   ("Resistance:"),    fbxResistance(0, 1.0f),
    lblStep         ("Step:"),          fbxStep(0, 1.0f),
    lblAttractions  ("Attractions"),    fbxAttractions(9, std::vector<Fbx>(9, Fbx(-1.0f, 1.0f))),
    lblTypeRatios   ("Type Ratio"),     fbxTypeRatios(9, Ibx(0, 65536)),

    grpCopyPreloadedSettings    ("Copy Settings"),
    tglgrpCopyPreloadedSettings ("Default;Custom"),
    flsvCopyDefaultSettings     ("./settings/default/"),
    flsvCopyCustomSettings      ("./settings/custom/"),
    btnCopyPreloadedSettings    ("Copy"),

    btnValidateSettings("Validate"),
    btnSaveSettings    ("Save"),

    grpErrors("Errors"),
    lsvErrors("TEST_ERROR_1;TEST_ERROR_2;TEST_ERROR_3;TEST_ERROR_4;TEST_ERROR_5;TEST_ERROR_6;TEST_ERROR_7"),

    // UNIVERSAL WIDGETS

    grpFooter ("Actions"),
    btnExecute("Execute"),
    
    // METRICS

    W                   (GetScreenWidth()),
    H                   (GetScreenHeight()),

    U                   (16),
    M                   (8),

    tglbtnWidth         (4*U),
    flsvPreloadWidth    (16*U),
    flsvPreloadHeight   (12*U),
    fieldWidth          (4*U),
    inlineLabelWidth    (5*U),
    matrixFieldWidth    (3.5*U),
    flsvCopyWidth       (10*U),
    lsvErrorsHeight     (6*U),
    btnWidth            (4*U),

    headerRec           { 0, 0, 0, 0 },
    bodyRec             { 0, 0, 0, 0 },
    footerRec           { 0, 0, 0, 0 }

{
    // read preloaded default and custom settings
    readPreloadedSettings(flsvSelectDefaultSettings, defaultSettings);
    readPreloadedSettings(flsvSelectCustomSettings, customSettings);
    readPreloadedSettings(flsvCopyDefaultSettings, defaultSettings);
    readPreloadedSettings(flsvCopyCustomSettings, customSettings);
}

bool Launcher::run()
{
    if (W != GetScreenWidth() || H != GetScreenHeight()) SetWindowSize(W, H);
    
    BeginDrawing();
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    header();

    bool submitted = false;
    switch (tglgrpSettingsTab.activeToggle) {
        case 0: submitted = preloaded(); break;
        case 1: submitted = customised(); break;
        default: break;
    }

    EndDrawing();

    return !submitted;
}

ParticleLife::Settings& Launcher::getSettings()
{
    // TODO: return settings based on active toggle and selections
    return userCustomisedSettings;
}

void Launcher::header()
{
    Rectangle r;
    
    headerRec = Rectangle { M, M, W-2*M, U+2*M };
    grpHeader.update(headerRec);

    r = Rectangle { headerRec.x + M, headerRec.y + M, 4*U, U };
    tglgrpSettingsTab.update(r);
}

bool Launcher::preloaded()
{
    // set body position
    bodyRec.x = headerRec.x;
    bodyRec.y = headerRec.y + headerRec.height + M;

    // set preloaded settings groupbox dimensions
    Rectangle preloadedSettingsRec = {
        bodyRec.x,
        bodyRec.y,
        flsvPreloadWidth + 2*M,
        flsvPreloadHeight + U + 3*M
    };

    // set body size
    bodyRec.width = preloadedSettingsRec.width;
    bodyRec.height = preloadedSettingsRec.height;

    // set footer dimensions
    footerRec = {
        bodyRec.x,
        bodyRec.y + bodyRec.height + M,
        bodyRec.width,
        U + 2*M
    };

    // update window size variables
    W = bodyRec.width + 2*M;
    H = footerRec.y + footerRec.height + M;

    // // visualise formatting
    // unsigned char C = 255, A = 31;
    // DrawRectangleRec(headerRec,            Color{ 0, 0, 0, A });
    // DrawRectangleRec(bodyRec,              Color{ 0, 0, 0, A });
    // // DrawRectangleRec(preloadedSettingsRec, Color{ C, 0, 0, A });
    // DrawRectangleRec(footerRec,            Color{ 0, 0, 0, A });

    // preloaded settings groupbox
    grpPreloadedSettings.update(preloadedSettingsRec);

    Rectangle r;

    // update select default/custom settings toggle group
    r = { preloadedSettingsRec.x + M, preloadedSettingsRec.y + M, tglbtnWidth, U };
    tglgrpSelectPreloadedSettings.update(r);

    // update settings file list view
    r = { r.x, r.y + r.height + M, flsvPreloadWidth, flsvPreloadHeight };
    switch (tglgrpSelectPreloadedSettings.activeToggle) {
        case 0: flsvSelectDefaultSettings.update(r); break;
        case 1: flsvSelectCustomSettings.update(r); break;
        default: break;
    }

    // FOOTER

    // footer groupbox
    grpFooter.update(footerRec);

    // validate button
    r = { footerRec.x + footerRec.width/2 - btnWidth/2, footerRec.y + M, btnWidth, U };
    btnExecute.update(r);

    // TODO: return true if settings are selected and user presses execute

    return false;
}

bool Launcher::customised()
{
    // widget related counts
    int T = 9;       // number of types     TODO: get from customisedSettings
    int singles = 7; // number of single settings in col1
    int multis = 2;  // number of multi settings in col2

    // set body position
    bodyRec.x = headerRec.x;
    bodyRec.y = headerRec.y + headerRec.height + M;

    // set custom settings columns
    Rectangle customiseColumn1 = {
        bodyRec.x,
        bodyRec.y,
        inlineLabelWidth + fieldWidth + 3*M,
        singles*(U+M)+M
    };
    Rectangle customiseColumn2 = {
        customiseColumn1.x + customiseColumn1.width,
        customiseColumn1.y,
        T*(matrixFieldWidth + M) + M,
        T*(U+M) + multis*(U+M) + 2*M
    };

    // update body height
    bodyRec.height = std::max(customiseColumn1.height, customiseColumn2.height);

    // set copy settings rows
    Rectangle copyRow1 = {
        customiseColumn2.x + customiseColumn2.width + M,
        customiseColumn2.y,
        flsvCopyWidth + 2*M,
        bodyRec.height - U - 2*M
    };
    Rectangle copyRow2 = {
        copyRow1.x,
        copyRow1.y + copyRow1.height,
        copyRow1.width,
        bodyRec.height - copyRow1.height
    };

    // set custom settings groupbox dimensions
    Rectangle customSettingsRec = {
        customiseColumn1.x,
        customiseColumn2.y,
        customiseColumn1.width + customiseColumn2.width,
        bodyRec.height
    };

    // set copy settings groupbox dimensions
    Rectangle copySettingsRec = {
        customSettingsRec.x + customSettingsRec.width + M,
        customSettingsRec.y,
        flsvCopyWidth + 2*M,
        bodyRec.height
    };

    // update body width
    bodyRec.width = -customSettingsRec.x + copySettingsRec.x + copySettingsRec.width;

    // set error widget dimensions
    Rectangle errorsRec = { 0, 0, 0, 0 };
    Rectangle lsvErrorsRec = { 0, 0, 0, 0 };
    if (lsvErrors.text[0] != '\0') {
        errorsRec = {
            bodyRec.x,
            bodyRec.y + bodyRec.height + M,
            bodyRec.width,
            lsvErrorsHeight + 2*M
        };
        lsvErrorsRec = {
            errorsRec.x + M,
            errorsRec.y + M,
            errorsRec.width - 2*M,
            errorsRec.height - 2*M
        };
        bodyRec.height += errorsRec.height + M; // update body height
    }

    // set footer dimensions
    footerRec = {
        bodyRec.x,
        bodyRec.y + bodyRec.height + M,
        bodyRec.width,
        U + 2*M
    };

    // update window size variables
    W = bodyRec.width + 2*M;
    H = footerRec.y + footerRec.height + M;

    // // visualise formatting
    // unsigned char C = 255, A = 31;
    // DrawRectangleRec(headerRec,         Color{ 0, 0, 0, A });
    // DrawRectangleRec(bodyRec,           Color{ 0, 0, 0, A });
    // DrawRectangleRec(customSettingsRec, Color{ 0, 0, 0, A });
    // DrawRectangleRec(customiseColumn1,  Color{ C, 0, 0, A });
    // DrawRectangleRec(customiseColumn2,  Color{ 0, C, 0, A });
    // DrawRectangleRec(copySettingsRec,   Color{ 0, 0, 0, A });
    // DrawRectangleRec(copyRow1,          Color{ C, C, 0, A });
    // DrawRectangleRec(copyRow2,          Color{ 0, C, C, A });
    // DrawRectangleRec(errorsRec,         Color{ C, 0, C, A });
    // DrawRectangleRec(footerRec,         Color{ 0, 0, 0, A });

    Rectangle r; // rectangle buffer

    // EDIT CUSTOM SETTINGS

    grpCustomisedSettings.update(customSettingsRec);

    // col1: single setting column labels
    r = { customSettingsRec.x+M, customSettingsRec.y+M, inlineLabelWidth, U };
    lblName.update(r);          r.y += M + U;
    lblTypes.update(r);         r.y += M + U;
    lblSize.update(r);          r.y += M + U;
    lblCount.update(r);         r.y += M + U;
    lblInnerRadius.update(r);   r.y += M + U;
    lblResistance.update(r);    r.y += M + U;
    lblStep.update(r);          r.y += M + U;

    // col1: single setting column fields
    r = { customSettingsRec.x + 2*M + inlineLabelWidth, customSettingsRec.y + M, fieldWidth, U };
    tbxName.update(r);          r.y += M + U;
    ibxTypes.update(r);         r.y += M + U;
    ibxSize.update(r);          r.y += M + U;
    ibxCount.update(r);         r.y += M + U;
    fbxInnerRadius.update(r);   r.y += M + U;
    fbxResistance.update(r);    r.y += M + U;
    fbxStep.update(r);          r.y += M + U;

    // col2: attraction matrix
    r = { customiseColumn2.x+M, customiseColumn2.y+M, customiseColumn2.width-2*M, U };
    lblAttractions.update(r);
    r = { r.x, r.y+U, matrixFieldWidth, U };
    for (int i = 0; i < T; i++) {
        for (int j = 0; j < T; j++) {
            fbxAttractions[i][j].update(r);
            r.x += matrixFieldWidth + M;
        } r.x = customiseColumn2.x + M;
          r.y += U + M;
    }

    // col2: type ratios
    r = { customiseColumn2.x+M, r.y, customiseColumn2.width-2*M, U };
    lblTypeRatios.update(r);
    r = { r.x, r.y+U, matrixFieldWidth, U };
    for (int i = 0; i < T; i++) {
        fbxTypeRatios[i].update(r);
        r.x += matrixFieldWidth + M;
    }

    // COPY PRELOADED SETTINGS

    grpCopyPreloadedSettings.update(copySettingsRec);

    // copy default/custom toggle group
    r = { copySettingsRec.x + M, copySettingsRec.y + M, tglbtnWidth, U };
    tglgrpCopyPreloadedSettings.update(r);

    // update copy settings file list view
    r = { r.x,
          r.y + r.height + M,
          flsvCopyWidth,
          copyRow2.y - (r.y + r.height + M)
          
    }; switch (tglgrpCopyPreloadedSettings.activeToggle) {
        case 0: flsvCopyDefaultSettings.update(r); break;
        case 1: flsvCopyCustomSettings.update(r); break;
        default: break;
    }

    // copy settings button
    r = { copyRow2.x + M, copyRow2.y + M, btnWidth, U };
    btnCopyPreloadedSettings.update(r);

    // ERRORS

    grpErrors.update(errorsRec);
    lsvErrors.update(lsvErrorsRec);

    // FOOTER

    // footer groupbox
    grpFooter.update(footerRec);

    // validate button
    r = { footerRec.x + M, footerRec.y + M, btnWidth, U };
    btnValidateSettings.update(r);

    // save button
    r.x += btnWidth + M;
    btnSaveSettings.update(r);

    // execute button
    r = { footerRec.x + footerRec.width - btnWidth - M, footerRec.y + M, btnWidth, U };
    btnExecute.update(r);
    
    // TODO: return true if valid settings are entered and user presses execute

    return false;
}

void Launcher::readPreloadedSettings(FLsv& flsv, std::vector<ParticleLife::Settings>& settings)
{
    for (const auto& dirEntry : fs::directory_iterator(flsv.dirPath)) {
        try {
            settings.push_back(ParticleLife::Settings(dirEntry));
        } catch (std::exception& e) { 
            std::cout << "Failed to load settings from " << dirEntry.path() << ": " << e.what() << std::endl;
        }
    }
    flsv.updateContents();
}

void Launcher::validateSettings(ParticleLife::Settings& settings)
{

}

void Launcher::saveSettings(ParticleLife::Settings& settings)
{

}