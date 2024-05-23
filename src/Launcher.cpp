#include "Launcher.hpp"

#include <raylib.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <dirent.h>
#include <regex>

#include <filesystem>
namespace fs = std::filesystem;

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


#define PARTICLE_LIFE_MIN_MAX_TYPES         PARTICLE_LIFE_MIN_TYPES, PARTICLE_LIFE_MAX_TYPES
#define PARTICLE_LIFE_MIN_MAX_GRID_SIZE     PARTICLE_LIFE_MIN_GRID_SIZE, PARTICLE_LIFE_MAX_GRID_SIZE
#define PARTICLE_LIFE_MIN_MAX_COUNT         PARTICLE_LIFE_MIN_COUNT, PARTICLE_LIFE_MAX_COUNT
#define PARTICLE_LIFE_MIN_MAX_INNER_RADIUS  PARTICLE_LIFE_MIN_INNER_RADIUS, PARTICLE_LIFE_MAX_INNER_RADIUS
#define PARTICLE_LIFE_MIN_MAX_STEP          PARTICLE_LIFE_MIN_STEP, PARTICLE_LIFE_MAX_STEP
#define PARTICLE_LIFE_MIN_MAX_RESISTANCE    PARTICLE_LIFE_MIN_RESISTANCE, PARTICLE_LIFE_MAX_RESISTANCE
#define PARTICLE_LIFE_MIN_MAX_ATTRACTION    PARTICLE_LIFE_MIN_ATTRACTION, PARTICLE_LIFE_MAX_ATTRACTION
#define PARTICLE_LIFE_MIN_MAX_SEED          PARTICLE_LIFE_MIN_SEED, PARTICLE_LIFE_MAX_SEED
#define PARTICLE_LIFE_MIN_MAX_RATIO         PARTICLE_LIFE_MIN_RATIO, PARTICLE_LIFE_MAX_RATIO



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
        strncat(text, entry->d_name, strlen(entry->d_name) - 4);
        strcat(text, ";");
        
    }
    closedir(dir);
    if (strlen(text) > 0)
        text[strlen(text)-1] = '\0';
    else strcpy(text, "No files found...");
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



Launcher::Launcher() :

    // PARTICLE LIFE SETTINGS DATA

    // add defaulted settings to first settings entries
    defaultSettings(std::vector<ParticleLife::Settings>(1, ParticleLife::Settings())),
    customSettings (std::vector<ParticleLife::Settings>(1, ParticleLife::Settings())),

    userCustomisedSettings(ParticleLife::Settings()),  // defaults to default
    currentSettingsPtr(&userCustomisedSettings),       // defaults guaranteed instance

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

    lblName("Name:"),                   tbxName(),  // NOTE: MIN_MAX substitutes two arguments:  min, max
    lblTypes("Types:"),                 ibxTypes(PARTICLE_LIFE_MIN_MAX_TYPES),              
    lblSize("Size:"),                   ibxSize(PARTICLE_LIFE_MIN_MAX_GRID_SIZE),           
    lblCount("Count:"),                 ibxCount(PARTICLE_LIFE_MIN_MAX_COUNT),              
    lblInnerRadius("Inner Radius:"),    fbxInnerRadius(PARTICLE_LIFE_MIN_MAX_INNER_RADIUS), 
    lblResistance("Resistance:"),       fbxResistance(PARTICLE_LIFE_MIN_MAX_RESISTANCE),
    lblStep("Step:"),                   fbxStep(PARTICLE_LIFE_MIN_MAX_STEP),
    lblAttractions("Attractions"),      fbxAttractions(PARTICLE_LIFE_MAX_TYPES, std::vector<Fbx>(PARTICLE_LIFE_MAX_TYPES, Fbx(PARTICLE_LIFE_MIN_MAX_ATTRACTION))),
    lblTypeRatios("Type Ratio"),        fbxTypeRatios(PARTICLE_LIFE_MAX_TYPES, Ibx(PARTICLE_LIFE_MIN_MAX_RATIO)),

    grpCopyPreloadedSettings("Copy Settings"),
    tglgrpCopyPreloadedSettings("Default;Custom"),

    flsvCopyDefaultSettings("./settings/default/"),
    flsvCopyCustomSettings("./settings/custom/"),

    btnCopyPreloadedSettings("Copy"),

    btnValidateCustomSettings("Validate"),
    btnSaveCustomSettings    ("Save"),

    grpErrors("Errors"),
    lsvErrors(""),

    // UNIVERSAL WIDGETS

    grpFooter ("Actions"),
    btnExecute("Execute"),
    
    // METRICS

    windowWidth (GetScreenWidth()),
    windowHeight(GetScreenHeight()),
    headerRec { 0, 0, 0, 0 },
    bodyRec   { 0, 0, 0, 0 },
    footerRec { 0, 0, 0, 0 },

    U                 ( 16    ),
    M                 ( 8     ),
    tglbtnWidth       ( 4*U   ),
    flsvPreloadWidth  ( 16*U  ),
    flsvPreloadHeight ( 12*U  ),
    fieldWidth        ( 4*U   ),
    inlineLabelWidth  ( 5*U   ),
    matrixFieldWidth  ( 3.5*U ),
    flsvCopyWidth     ( 10*U  ),
    lsvErrorsHeight   ( 6*U   ),
    btnWidth          ( 4* U  )

{
    // read preloaded default and custom settings
    readPreloadedSettings(flsvSelectDefaultSettings, defaultSettings);
    readPreloadedSettings(flsvSelectCustomSettings, customSettings);
    readPreloadedSettings(flsvCopyDefaultSettings, defaultSettings);
    readPreloadedSettings(flsvCopyCustomSettings, customSettings);
}

bool Launcher::run()
{
    if (windowWidth != GetScreenWidth() || windowHeight != GetScreenHeight())
        SetWindowSize(windowWidth, windowHeight);

    auto previousSettings = currentSettingsPtr;

    if (tglgrpSettingsTab.activeToggle == 0)
        currentSettingsPtr = (tglgrpSelectPreloadedSettings.activeToggle == 0)
          ? &defaultSettings[flsvSelectDefaultSettings.activeIdx]
          : &customSettings[flsvSelectCustomSettings.activeIdx];
    else currentSettingsPtr = &userCustomisedSettings;

    if (previousSettings != currentSettingsPtr)
        std::cout << (currentSettingsPtr == &userCustomisedSettings ? "customise" : "load") << " settings " << std::endl << std::endl,
        std::cout << *currentSettingsPtr << std::endl;
    
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
    return *currentSettingsPtr;
}

void Launcher::header()
{
    Rectangle r;
    
    headerRec = Rectangle { M, M, windowWidth-2*M, U+2*M };
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
    windowWidth = bodyRec.width + 2*M;
    windowHeight = footerRec.y + footerRec.height + M;

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
    if (btnExecute.update(r)) {
        
    }

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
    windowWidth = bodyRec.width + 2*M;
    windowHeight = footerRec.y + footerRec.height + M;

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

    float midGap = (footerRec.width - btnWidth) / 2;
    r = { 0, footerRec.y + M, btnWidth, U };

    // validate button
    r.x = footerRec.x + (midGap - btnWidth) / 2;
    if (btnValidateCustomSettings.update(r)) {
        bool validity = validateInputSettings();
    }

    // save button
    r.x = footerRec.x + midGap;
    if (btnSaveCustomSettings.update(r)) {
        bool saved = saveCustomisedSettings();
    }

    // execute button
    r.x = footerRec.x + footerRec.width - (midGap + btnWidth) / 2;
    if (btnExecute.update(r)) {
        bool validity = validateInputSettings();
        // TODO: clean oversized vectors
        return validity;
    }

    // continue running launcher
    return false;
}

void Launcher::readPreloadedSettings(FLsv& flsv, std::vector<ParticleLife::Settings>& settings)
{
    settings.clear();
    for (const auto& dirEntry : fs::directory_iterator(flsv.dirPath)) {
        try {
            settings.push_back(ParticleLife::Settings(dirEntry));
        } catch (std::exception& e) { 
            std::cout << "Failed to load settings from " << dirEntry.path() << ": " << e.what() << std::endl;
        }
    }
    if (settings.empty())
        settings.push_back(ParticleLife::Settings());
    flsv.updateContents();
}

bool Launcher::validateInputSettings()
{
    // current input errors
    std::vector<std::string> errors;

    // field buffer to used to format error messages
    std::string field;

    // VALIDATE SETTINGS INPUTS

    // validate name

    field = "Name: ";
    std::vector<char> validSpecialChars = { '-', '_', '(', ')' };
    int nameLength = strlen(tbxName.text);
    if (nameLength == 0)
        errors.push_back(field + "cannot be empty.");
    if (nameLength > PARTICLE_LIFE_MAX_NAME_LENGTH)
        errors.push_back(field + "must have less than " + std::to_string(PARTICLE_LIFE_MAX_NAME_LENGTH) + " characters.");
    int i = 0;
    for (; i < nameLength; i++) {
        char c = tbxName.text[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
            continue;
        for (char sc : validSpecialChars)
            if (c == sc) break;
    }if (i != nameLength)
        errors.push_back(field + "contains invalid characters.");

    // validate types

    field = "Types: ";
    if (ibxTypes.value < PARTICLE_LIFE_MIN_TYPES)
        errors.push_back(field + "must exceed " + std::to_string(PARTICLE_LIFE_MIN_TYPES) + ".");
    if (ibxTypes.value > PARTICLE_LIFE_MAX_TYPES)
        errors.push_back(field + "cannot exceed " + std::to_string(PARTICLE_LIFE_MAX_TYPES) + ".");

    // validate size

    field = "Size: ";
    if (ibxSize.value < PARTICLE_LIFE_MIN_GRID_SIZE)
        errors.push_back(field + "must exceed " + std::to_string(PARTICLE_LIFE_MIN_GRID_SIZE) + ".");
    if (ibxSize.value > PARTICLE_LIFE_MAX_GRID_SIZE)
        errors.push_back(field + "cannot exceed " + std::to_string(PARTICLE_LIFE_MAX_GRID_SIZE) + ".");

    // validate count

    field = "Count: ";
    if (ibxCount.value < PARTICLE_LIFE_MIN_COUNT)
        errors.push_back(field + "must exceed " + std::to_string(PARTICLE_LIFE_MIN_COUNT) + ".");
    if (ibxCount.value > PARTICLE_LIFE_MAX_COUNT)
        errors.push_back(field + "cannot exceed " + std::to_string(PARTICLE_LIFE_MAX_COUNT) + ".");

    // validate inner radius

    field = "Inner Radius: ";
    if (fbxInnerRadius.value < PARTICLE_LIFE_MIN_INNER_RADIUS)
        errors.push_back(field + "must exceed " + std::to_string(PARTICLE_LIFE_MIN_INNER_RADIUS) + ".");
    if (fbxInnerRadius.value > PARTICLE_LIFE_MAX_INNER_RADIUS)
        errors.push_back(field + "cannot exceed " + std::to_string(PARTICLE_LIFE_MAX_INNER_RADIUS) + ".");
    
    // validate resistance

    field = "Resistance: ";
    if (fbxResistance.value < PARTICLE_LIFE_MIN_RESISTANCE)
        errors.push_back(field + "must exceed " + std::to_string(PARTICLE_LIFE_MIN_RESISTANCE) + ".");
    if (fbxResistance.value > PARTICLE_LIFE_MAX_RESISTANCE)
        errors.push_back(field + "cannot exceed " + std::to_string(PARTICLE_LIFE_MAX_RESISTANCE) + ".");

    // validate step

    field = "Step: ";
    if (fbxStep.value < PARTICLE_LIFE_MIN_STEP)
        errors.push_back(field + "must exceed " + std::to_string(PARTICLE_LIFE_MIN_STEP) + ".");
    if (fbxStep.value > PARTICLE_LIFE_MAX_STEP)
        errors.push_back(field + "cannot exceed " + std::to_string(PARTICLE_LIFE_MAX_STEP) + ".");

    // valiudate attractions

    for (int i = 0; i < ibxTypes.value; i++) {
        for (int j = 0; j < ibxTypes.value; j++) {
            field = "Attraction["+std::to_string(i)+"]["+std::to_string(j)+"]: ";
            if (fbxAttractions[i][j].value < PARTICLE_LIFE_MIN_ATTRACTION)
                errors.push_back(field + "must exceed " + std::to_string(PARTICLE_LIFE_MIN_ATTRACTION) + ".");
            if (fbxAttractions[i][j].value > PARTICLE_LIFE_MAX_ATTRACTION)
                errors.push_back(field + "must exceed " + std::to_string(PARTICLE_LIFE_MAX_ATTRACTION) + ".");
        }
    }

    // validate type ratios

    int ratioSum = 0;
    for (int i = 0; i < ibxTypes.value; i++) {
        field = "TypeRatio["+std::to_string(i)+"]: ";
        if (fbxTypeRatios[i].value < PARTICLE_LIFE_MIN_RATIO)
            errors.push_back(field + "must exceed " + std::to_string(PARTICLE_LIFE_MIN_RATIO) + ".");
        if (fbxTypeRatios[i].value > PARTICLE_LIFE_MAX_RATIO)
            errors.push_back(field + "cannot exceed " + std::to_string(PARTICLE_LIFE_MAX_RATIO) + ".");
        ratioSum += fbxTypeRatios[i].value;
    } if (ratioSum > ibxCount.value)
        errors.push_back("Type Ratios: sum must not exceed count.");

    // UPDATE ERROR DISPLAY

    lsvErrors.text[0] = '\0';
    i = 0;
    if (errors.size() > 0) {
        for (char c : errors[0]) lsvErrors.text[i++] = c;
        if (errors.size() > 1) {
            for (int j = 1; j < errors.size(); j++) {
                lsvErrors.text[i++] = ';';
                for (char c : errors[j])
                    lsvErrors.text[i++] = c;
            }
        } lsvErrors.text[i] = '\0';
    }

    // return true if no errors
    return errors.size() == 0;
}

bool Launcher::saveCustomisedSettings()
{
    // // validate settings
    // if (!validateInputSettings()) return false;

    return false;
}
