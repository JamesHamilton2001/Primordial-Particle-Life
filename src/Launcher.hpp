#ifndef LAUNCHER_HPP
#define LAUNCHER_HPP

#include "ParticleLife.hpp"
#include "GuiWrapper.hpp"

#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <limits>


/*
TODO:
 - add warnings widget for seed/ratio/particles shenanigans
 - copy to customised settings
 - create widget for custom seed
 - update particle life setting object to contatin type ratios
*/


class Launcher
{
  public:

    Launcher();
    
    // returns true on execute button press with valid settings, otherwise false
    bool run();

    // used to access the decided settings after the launcher has run
    ParticleLife::Settings getSettings();

  private:

    // PARTICLE LIFE SETTINGS DATA

    std::vector<ParticleLife::Settings> defaultSettings;    // hard copy of all default settings
    std::vector<ParticleLife::Settings> customSettings;     // hard copy of all custom settings

    ParticleLife::Settings userCustomisedSettings;  // user customised settings

    // HEADER WIDGETS

    Grp grpHeader;
    TglGrp tglgrpSettingsTab;

    // PRELOADED SETTINGS TAB WIDGETS

    Grp grpPreloadedSettings;

    TglGrp tglgrpSelectPreloadedSettings;
    FLsv flsvSelectDefaultSettings;
    FLsv flsvSelectCustomSettings;

    // CUSTOMISED SETTINGS TAB WIDGETS

    Grp grpCustomisedSettings;                                         // customised settings group
    Lbl lblName;        Tbx tbxName;                                   // name text box
    Lbl lblTypes;       Ibx ibxTypes;                                  // types int box
    Lbl lblSize;        Ibx ibxSize;                                   // size int box
    Lbl lblCount;       Ibx ibxCount;                                  // count int box
    Lbl lblInnerRadius; Fbx fbxInnerRadius;                            // inner radius float box
    Lbl lblResistance;  Fbx fbxResistance;                             // resistance float box
    Lbl lblStep;        Fbx fbxStep;                                   // step float box
    Lbl lblSeed;        Ibx ibxSeed;                                   // seed int box
    Lbl lblAttractions; std::vector<std::vector<Fbx>> fbxAttractions;  // attractions matrix float boxes
    Lbl lblTypeRatios;  std::vector<Ibx> fbxTypeRatios;                // type ratios int boxes

    Grp grpCopyPreloadedSettings;       // copy preloaded settings group
    TglGrp tglgrpCopyPreloadedSettings; // toggle default/custom settings to copy
    FLsv flsvCopyDefaultSettings;       // file list view to copy default settings for customisation
    FLsv flsvCopyCustomSettings;        // copy custom settings file list view
    Btn btnCopyPreloadedSettings;       // copy preloaded settings into customised settings button

    Btn btnValidateCustomSettings; // validate settings button
    Btn btnSaveCustomSettings;     // save settings button

    Grp grpErrors;  // errors group box
    Lsv lsvErrors;  // list view of current errors

    // UNIVRSAL WIDGETS

    Grp grpFooter;   // footer group box
    Btn btnExecute;  

    // METRICS

    int windowWidth;      // window width
    int windowHeight;     // window height
    Rectangle headerRec;  // header dimensions
    Rectangle bodyRec;    // body dimensions
    Rectangle footerRec;  // footer dimensions

    const float U;                  // widget unit of measure
    const float M;                  // widget gap unit of measure
    const float tglbtnWidth;        // width of a toggle button, also for in toggle groups
    const float flsvPreloadWidth;   // width of a file list view to load settings
    const float flsvPreloadHeight;  // height of a file list view to load settings
    const float fieldWidth;         // width of a field representing a single value
    const float inlineLabelWidth;   // width of a label representing a single value
    const float matrixFieldWidth;   // width of a field representing a value in a matrix
    const float flsvCopyWidth;      // width of a file list view to copy settings
    const float lsvErrorsHeight;    // height of a list view for errors
    const float btnWidth;           // width of a button

    // GUI HANDLER METHODS

    void header();      // handles header widgets
    bool preloaded();   // handles preloaded tab widgets, returns true on click execute preloaded settings, otherwise false
    bool customised();  // handles customise tab widgets, returns true execution of valid custom settings, otherwise false

    // UTIL METHODS

    bool validateInputSettings();
    bool saveCustomisedSettings();

    void readPreloadedSettings(FLsv& flsv, std::vector<ParticleLife::Settings>& settings);
    
};



#endif