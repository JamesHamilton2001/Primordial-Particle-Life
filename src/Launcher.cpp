#include "Launcher.hpp"

#include <raylib.h>
#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;



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
    lblTypes("Types:"),                 ibxTypes(PARTICLELIFE_MIN_TYPES, PARTICLELIFE_MAX_TYPES),              
    lblSize("Size:"),                   ibxSize(PARTICLELIFE_MIN_GRID_SIZE, PARTICLELIFE_MAX_GRID_SIZE),           
    lblCount("Count:"),                 ibxCount(PARTICLELIFE_MIN_COUNT, PARTICLELIFE_MAX_COUNT),              
    lblInnerRadius("Inner Radius:"),    fbxInnerRadius(PARTICLELIFE_MIN_INNER_RADIUS, PARTICLELIFE_MAX_INNER_RADIUS), 
    lblResistance("Resistance:"),       fbxResistance(PARTICLELIFE_MIN_RESISTANCE, PARTICLELIFE_MAX_RESISTANCE),
    lblStep("Step:"),                   fbxStep(PARTICLELIFE_MIN_STEP, PARTICLELIFE_MAX_STEP),
    lblAttractions("Attractions"),      fbxAttractions(PARTICLELIFE_MAX_TYPES, std::vector<Fbx>(PARTICLELIFE_MAX_TYPES, Fbx(PARTICLELIFE_MIN_ATTRACTION, PARTICLELIFE_MAX_ATTRACTION))),
    lblTypeRatios("Type Ratio"),        fbxTypeRatios(PARTICLELIFE_MAX_TYPES, Ibx(PARTICLELIFE_MIN_RATIO, PARTICLELIFE_MAX_RATIO)),

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

    if (tglgrpSettingsTab.activeToggle == 0)
        currentSettingsPtr = (tglgrpSelectPreloadedSettings.activeToggle == 0)
          ? &defaultSettings[flsvSelectDefaultSettings.activeIdx]
          : &customSettings[flsvSelectCustomSettings.activeIdx];
    else currentSettingsPtr = &userCustomisedSettings;
    
    BeginDrawing();
    // ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    ClearBackground(getGuiBackgroundColor());

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
    int T = std::clamp(ibxTypes.value, PARTICLELIFE_MIN_TYPES, PARTICLELIFE_MAX_TYPES);
    int singles = 7;        // number of single settings in col1
    int multis = 2;         // number of multi settings in col2

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

    if (lsvErrors.text[0] != '\0')
        grpErrors.update(errorsRec),
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
    if (nameLength > PARTICLELIFE_MAX_NAME_LENGTH)
        errors.push_back(field + "must have less than " + std::to_string(PARTICLELIFE_MAX_NAME_LENGTH) + " characters.");
    int i = 0;
    for (; i < nameLength; i++) {
        char c = tbxName.text[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
            continue;
        for (char sc : validSpecialChars)
            if (c == sc) {break;}
    }if (i != nameLength)
        errors.push_back(field + "contains invalid characters.");

    // validate types

    field = "Types: ";
    if (ibxTypes.value < PARTICLELIFE_MIN_TYPES)
        errors.push_back(field + "must exceed " + std::to_string(PARTICLELIFE_MIN_TYPES) + ".");
    if (ibxTypes.value > PARTICLELIFE_MAX_TYPES)
        errors.push_back(field + "cannot exceed " + std::to_string(PARTICLELIFE_MAX_TYPES) + ".");

    // validate size

    field = "Size: ";
    if (ibxSize.value < PARTICLELIFE_MIN_GRID_SIZE)
        errors.push_back(field + "must exceed " + std::to_string(PARTICLELIFE_MIN_GRID_SIZE) + ".");
    if (ibxSize.value > PARTICLELIFE_MAX_GRID_SIZE)
        errors.push_back(field + "cannot exceed " + std::to_string(PARTICLELIFE_MAX_GRID_SIZE) + ".");

    // validate count

    field = "Count: ";
    if (ibxCount.value < ibxTypes.value)
        errors.push_back(field + "must be greater than or equal to types.");
    if (ibxCount.value < PARTICLELIFE_MIN_COUNT)
        errors.push_back(field + "must exceed " + std::to_string(PARTICLELIFE_MIN_COUNT) + ".");
    if (ibxCount.value > PARTICLELIFE_MAX_COUNT)
        errors.push_back(field + "cannot exceed " + std::to_string(PARTICLELIFE_MAX_COUNT) + ".");


    // validate inner radius

    field = "Inner Radius: ";
    if (fbxInnerRadius.value < PARTICLELIFE_MIN_INNER_RADIUS)
        errors.push_back(field + "must exceed " + std::to_string(PARTICLELIFE_MIN_INNER_RADIUS) + ".");
    if (fbxInnerRadius.value > PARTICLELIFE_MAX_INNER_RADIUS)
        errors.push_back(field + "cannot exceed " + std::to_string(PARTICLELIFE_MAX_INNER_RADIUS) + ".");
    
    // validate resistance

    field = "Resistance: ";
    if (fbxResistance.value < PARTICLELIFE_MIN_RESISTANCE)
        errors.push_back(field + "must exceed " + std::to_string(PARTICLELIFE_MIN_RESISTANCE) + ".");
    if (fbxResistance.value > PARTICLELIFE_MAX_RESISTANCE)
        errors.push_back(field + "cannot exceed " + std::to_string(PARTICLELIFE_MAX_RESISTANCE) + ".");

    // validate step

    field = "Step: ";
    if (fbxStep.value < PARTICLELIFE_MIN_STEP)
        errors.push_back(field + "must exceed " + std::to_string(PARTICLELIFE_MIN_STEP) + ".");
    if (fbxStep.value > PARTICLELIFE_MAX_STEP)
        errors.push_back(field + "cannot exceed " + std::to_string(PARTICLELIFE_MAX_STEP) + ".");

    // valiudate attractions

    for (int i = 0; i < ibxTypes.value; i++) {
        for (int j = 0; j < ibxTypes.value; j++) {
            field = "Attraction["+std::to_string(i)+"]["+std::to_string(j)+"]: ";
            if (fbxAttractions[i][j].value < PARTICLELIFE_MIN_ATTRACTION)
                errors.push_back(field + "must exceed " + std::to_string(PARTICLELIFE_MIN_ATTRACTION) + ".");
            if (fbxAttractions[i][j].value > PARTICLELIFE_MAX_ATTRACTION)
                errors.push_back(field + "must exceed " + std::to_string(PARTICLELIFE_MAX_ATTRACTION) + ".");
        }
    }

    // validate type ratios

    int ratioSum = 0;
    for (int i = 0; i < ibxTypes.value; i++) {
        field = "TypeRatio["+std::to_string(i)+"]: ";
        if (fbxTypeRatios[i].value < PARTICLELIFE_MIN_RATIO)
            errors.push_back(field + "must exceed " + std::to_string(PARTICLELIFE_MIN_RATIO) + ".");
        if (fbxTypeRatios[i].value > PARTICLELIFE_MAX_RATIO)
            errors.push_back(field + "cannot exceed " + std::to_string(PARTICLELIFE_MAX_RATIO) + ".");
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

    // return false if errors found
    if (errors.size() > 0) return false;

    // INSERT INPUT SETTINGS TO USER CUSTOMISED SETTINGS OBJECT

    userCustomisedSettings.name = std::string(tbxName.text);
    userCustomisedSettings.types = ibxTypes.value;
    userCustomisedSettings.size = ibxSize.value;
    userCustomisedSettings.count = ibxCount.value;
    userCustomisedSettings.innerRadius = fbxInnerRadius.value;
    userCustomisedSettings.resistance = fbxResistance.value;
    userCustomisedSettings.step = fbxStep.value;

    userCustomisedSettings.attractions.clear();
    for (int i = 0; i < ibxTypes.value; i++) {
        userCustomisedSettings.attractions.push_back(std::vector<float>());
        for (int j = 0; j < ibxTypes.value; j++)
            userCustomisedSettings.attractions[i].push_back(fbxAttractions[i][j].value);
    }
    // TODO: proper seed when widget implemented
    userCustomisedSettings.seed = (userCustomisedSettings.particles.size() > 0) ? -1 : 0;

    userCustomisedSettings.typeRatio.clear();
    for (int i = 0; i < ibxTypes.value; i++)
        userCustomisedSettings.typeRatio.push_back(fbxTypeRatios[i].value);
    
    // TODO: take custom input for particles once widget(s) implemented
    if (userCustomisedSettings.seed != -1)
        userCustomisedSettings.particles.clear();

    std::cout << userCustomisedSettings << std::endl;
    // return true if no errors
    return true;
}

bool Launcher::saveCustomisedSettings()
{
    // validate settings
    if (!validateInputSettings()) return false;

    return false;
}
