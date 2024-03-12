#include "Launcher.hpp"

#include <raylib.h>
#include <iostream>
#include <string>

#include <filesystem>
namespace fs = std::filesystem;

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


Launcher::Launcher() :
        launch(false),
        customSetting(false)
{
    for (const auto& dirEntry : fs::directory_iterator("settings/default/")) {
        try {
            defaultSettings.push_back(ParticleLife::Settings(dirEntry));
        } catch (std::exception& e) { 
            std::cout << "Failed to load settings from " << dirEntry.path() << ": " << e.what() << std::endl;
        }
    }

    defaultDropDownText = defaultSettings[0].name;
    for (int i = 1; i < defaultSettings.size(); i++)
        defaultDropDownText += ';' + defaultSettings[i].name;

    defaultDropDownActive = false;
    defaultDropDownActiveIndex = 0;
    defaultDropDownEditMode = false;

}

bool Launcher::run()
{
    const float W = GetScreenWidth();   // screen width
    const float H = GetScreenHeight();  // screen height
    const float B = 32;                 // button unit
    const float M = 16;                 // margin unit

    Rectangle r;

    BeginDrawing();
    ClearBackground(LIGHTGRAY);

        // TOP BAR

            // Custom setting: Toggle
            r = Rectangle { W/4-1.5f*B, M, 3*B, B };
            GuiToggle(r, "Custom Settings", &customSetting);

            // Default Settings: Dropdown
            r.x = 3*W/4-1.5f*B;
            if (GuiDropdownBox(r, defaultDropDownText.c_str(), &defaultDropDownActiveIndex, defaultDropDownEditMode))
                defaultDropDownEditMode = !defaultDropDownEditMode;

        // BOTTOM BAR

            // Begin Simulation: Button
            r = Rectangle { W/2-B, H-1.5f*B-M, 3*B, B };
            launch = GuiButton(r, "Begin Simulation");   // launch button

    EndDrawing();

    return !launch;
}

ParticleLife::Settings& Launcher::getSettings()
{
    return defaultSettings[1];
}

