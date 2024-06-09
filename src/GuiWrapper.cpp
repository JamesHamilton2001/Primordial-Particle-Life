#include "GuiWrapper.hpp"

#include <iostream>
#include <string>
#include <algorithm>
#include <dirent.h>
#include <regex>

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


/*
TODO:
 - remove trailing zeros after last digit after point from float box
*/



Color getGuiBackgroundColor()
{
    return GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
}



void Wdgt::setText(const char* cstr)
{
    textSize = 0;
    while (cstr[textSize] != '\0' && textSize < textBufferSize) {
        text[textSize] = cstr[textSize];
        textSize++;
    } text[textSize] = '\0';
}

ostream& operator <<(ostream& os, const Wdgt& w)
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

ostream& operator <<(ostream& os, const Ibx& ibx)
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

    // handle external change of value
    if (value != oldValue) {
        text[0] = '\0';
        oldText[0] = '\0';
        snprintf(text, textBufferSize, "%f", value);
        strcpy(oldText, text);
        oldValue = value;
    }

    return false; // return false if contents not updated by user
}

ostream& operator <<(ostream& os, const Fbx& fbx)
{
    return os << static_cast<const Tbx&>(fbx) << ", " << fbx.value;
}

bool Lsv::update(Rectangle& rect)
{
    if (GuiListView(rect, text, &scrollIdx, &activeIdx))
        return true;
    return false;
}

ostream& operator <<(ostream& os, const Lsv& lsv)
{
    return os << lsv.scrollIdx << ", " << lsv.activeIdx << ", " << lsv.text;
}

void Flsv::updateContents()
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

ostream& operator <<(ostream& os, const TglGrp& tglGrp)
{
    return os << tglGrp.activeToggle << ", " << tglGrp.text;
}
