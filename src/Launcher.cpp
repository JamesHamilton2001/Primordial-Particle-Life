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
    defaultSettings(std::vector<Settings>(1, Settings())),
    customSettings (std::vector<Settings>(1, Settings())),

    userCustomisedSettings(Settings()),  // defaults to default

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
    lblSeed("Seed:"),                   ibxSeed(PARTICLELIFE_MIN_SEED, PARTICLELIFE_MAX_SEED),
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
    
    BeginDrawing();
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

Settings& Launcher::getSettings()
{
    if (tglgrpSettingsTab.activeToggle == 0) {
        if (tglgrpSelectPreloadedSettings.activeToggle == 0)
            return defaultSettings[flsvSelectDefaultSettings.activeIdx];
        else return customSettings[flsvSelectCustomSettings.activeIdx];
    }
    return userCustomisedSettings;
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
    return btnExecute.update(r);
}

bool Launcher::customised()
{
    // widget related counts
    int T = std::clamp(ibxTypes.value, PARTICLELIFE_MIN_TYPES, PARTICLELIFE_MAX_TYPES);
    int singles = 8;        // number of single settings in col1
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
    lblSeed.update(r);          r.y += M + U;

    // col1: single setting column fields
    r = { customSettingsRec.x + 2*M + inlineLabelWidth, customSettingsRec.y + M, fieldWidth, U };

    tbxName.update(r);          r.y += M + U;
    ibxTypes.update(r);         r.y += M + U;
    ibxSize.update(r);          r.y += M + U;
    ibxCount.update(r);         r.y += M + U;
    fbxInnerRadius.update(r);   r.y += M + U;
    fbxResistance.update(r);    r.y += M + U;
    fbxStep.update(r);          r.y += M + U;
    ibxSeed.update(r);          r.y += M + U;

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
    if (btnCopyPreloadedSettings.update(r)) {
        if (tglgrpCopyPreloadedSettings.activeToggle == 0)
            copyToCustomisedSettings(defaultSettings[flsvCopyDefaultSettings.activeIdx]);
        else copyToCustomisedSettings(customSettings[flsvCopyCustomSettings.activeIdx]);
    }

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

    // execute button, return true on valid input to execute
    r.x = footerRec.x + footerRec.width - (midGap + btnWidth) / 2;
    if (btnExecute.update(r))
        return validateInputSettings();

    // continue running launcher
    return false;
}

void Launcher::readPreloadedSettings(FLsv& flsv, std::vector<Settings>& settings)
{
    settings.clear();
    for (const auto& dirEntry : fs::directory_iterator(flsv.dirPath)) {
        try {
            settings.push_back(Settings(dirEntry));
        } catch (std::exception& e) { 
            std::cout << "Failed to load settings from " << dirEntry.path() << ": " << e.what() << std::endl;
        }
    }
    if (settings.empty())
        settings.push_back(Settings());
    flsv.activeIdx = 0;
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

    // valiate seed

    field = "Seed: ";
    if (ibxSeed.value < PARTICLELIFE_MIN_SEED)
        errors.push_back(field + "must exceed " + std::to_string(PARTICLELIFE_MIN_SEED) + ".");
    if (ibxSeed.value > PARTICLELIFE_MAX_SEED)
        errors.push_back(field + "cannot exceed " + std::to_string(PARTICLELIFE_MAX_SEED) + ".");

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

    field = "Type Ratios: ";
    int ratioSum = 0;
    for (int i = 0; i < ibxTypes.value; i++) {
        field = "TypeRatio["+std::to_string(i)+"]: ";
        if (fbxTypeRatios[i].value < PARTICLELIFE_MIN_RATIO)
            errors.push_back(field + "must exceed " + std::to_string(PARTICLELIFE_MIN_RATIO) + ".");
        if (fbxTypeRatios[i].value > PARTICLELIFE_MAX_RATIO)
            errors.push_back(field + "cannot exceed " + std::to_string(PARTICLELIFE_MAX_RATIO) + ".");
        ratioSum += fbxTypeRatios[i].value;
    } if (ratioSum > ibxCount.value)
        errors.push_back(field + "sum must not exceed count.");

    // validate paricles

    if (ibxSeed.value == -1) {
        if (userCustomisedSettings.particles.size() != ibxCount.value)
            errors.push_back("Copied particle data of length "+std::to_string(userCustomisedSettings.particles.size())+ " does not match count input");
    }

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

    Settings& s = userCustomisedSettings;

    s.name = std::string(tbxName.text);        // name
    s.types = ibxTypes.value;                  // types
    s.size = ibxSize.value;                    // size
    s.count = ibxCount.value;                  // count
    s.innerRadius = fbxInnerRadius.value;      // inner radius
    s.resistance = fbxResistance.value;        // resistance
    s.step = fbxStep.value;                    // step

    s.attractions.clear();                     // attractions
    for (int i = 0; i < ibxTypes.value; i++) {
        s.attractions.push_back(std::vector<float>());
        for (int j = 0; j < ibxTypes.value; j++)
            s.attractions[i].push_back(fbxAttractions[i][j].value);
    }

    s.seed = ibxSeed.value;                    // seed

    s.typeRatio.clear();                       // type ratios
    for (int i = 0; i < ibxTypes.value; i++)
        s.typeRatio.push_back(fbxTypeRatios[i].value);
    
    // TODO: take input for particles once widget(s) implemented
    if (s.seed != -1)                          // particles
        s.particles.clear();

    return true;  // false already returned on errors found
}

bool Launcher::saveCustomisedSettings()
{
    // validate settings
    if (!validateInputSettings()) return false;

    // create and/or open settings file, log error if failed
    std::ofstream file(Settings::customSettingsDir + userCustomisedSettings.name + ".txt");
    if (!file.is_open()) {
        std::string error = "Failed to save settings to " + userCustomisedSettings.name + ".txt";
        strcpy(lsvErrors.text, error.c_str());
        return false;
    }

    // write settings to file

    Settings& s = userCustomisedSettings;

    file << std::to_string(s.types) << '\n';        // types
    file << std::to_string(s.size) << '\n';         // size
    file << std::to_string(s.count) << '\n';        // count
    file << std::to_string(s.innerRadius) << '\n';  // innerRadius
    file << std::to_string(s.resistance) << '\n';   // resistance
    file << std::to_string(s.step) << '\n';         // step

    for (int i = 0; i < s.types; i++) {             // attractions
        file << std::to_string(s.typeRatio[i]);
        for (int j = 1; j < s.types; j++)
            file << ',' << std::to_string(s.typeRatio[j]);
        file << '\n';
    }

    file << s.seed << '\n';                         // seed (-1 for preloaded particles)

    if (s.seed != -1) {                             // type ratios
        for (int i = 0; i < s.types; i++)
            file << std::to_string(s.typeRatio[i]) << ',';
        file << '\n';
    }

    else {
        for (int i = 0; i < s.count; i++) {         // particles
            file << std::to_string(s.particles[i].type) << ','
                 << std::to_string(s.particles[i].pos.x) << ','
                 << std::to_string(s.particles[i].pos.y) << ','
                 << std::to_string(s.particles[i].vel.x) << ','
                 << std::to_string(s.particles[i].vel.y) << '\n';
        }
    }

    file.close();

    // update custom settings list views

    readPreloadedSettings(flsvSelectCustomSettings, customSettings);
    readPreloadedSettings(flsvCopyCustomSettings, customSettings);

    return true;
}

void Launcher::copyToCustomisedSettings(const Settings& settings)
{
    // copy the actual settings object over
    userCustomisedSettings = settings;
    
    // update the widgets with the new settings
    strcpy(tbxName.text, settings.name.c_str());
    ibxTypes.value = settings.types;
    ibxSize.value = settings.size;
    ibxCount.value = settings.count;
    fbxInnerRadius.value = settings.innerRadius;
    fbxResistance.value = settings.resistance;
    fbxStep.value = settings.step;
    ibxSeed.value = settings.seed;
    for (int i = 0; i < settings.types; i++)
        for (int j = 0; j < settings.types; j++)
            fbxAttractions[i][j].value = settings.attractions[i][j];
    for (int i = 0; i < settings.types; i++)
        fbxTypeRatios[i].value = settings.typeRatio[i];
}
