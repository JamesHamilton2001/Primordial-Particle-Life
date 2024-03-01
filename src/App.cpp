#include "App.hpp"

#include "Particle.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
namespace fs = std::filesystem;


App::App(int width, int height, int fpsTarget) :
    width(width),
    height(height),
    fpsTarget(fpsTarget),
    paused(false),
    drawGrid(true),
    drawGhosts(true),
    camera { Vector2 { float(width/2), float(height/2) },
             Vector2 { 0, 0 }, 0.0f, 25.0f             }
{
    // generate particle texture (64x64 white circle on transparent background)
    Image img = GenImageColor(64, 64, BLANK);
    ImageDrawCircle(&img, 32, 32, 32, WHITE);
    particleTexture = LoadTextureFromImage(img);
    UnloadImage(img);

    testStringConversions();

    loadSettings();
}

App::~App()
{
    UnloadTexture(particleTexture);
}


void App::update(ParticleLife& particleLife)
{
    // camera zoom on MOUSE_WHEEL
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        camera.target = GetScreenToWorld2D(camera.offset, camera);
        camera.zoom += (wheel * 2.0f);
        if (camera.zoom < 2.0f)
            camera.zoom = 2.0f;
    }
    // camera pan on HOLD_RIGHT_CLICK + DRAG
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        camera.target = Vector2Add(camera.target, Vector2Scale(GetMouseDelta(), -1.0f / camera.zoom));
    
    // print cell contents on PRESS_LEFT_CLICK
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        particleLife.printCellAtPos(mouseWorldPos);
    }

    // toggle pause on PRESS_SPACE
    if (IsKeyPressed(KEY_SPACE))
        paused = !paused;

    // toggle grid on PRESS_G
    if (IsKeyPressed(KEY_G))
        drawGrid = !drawGrid;

    if (IsKeyPressed(KEY_C))
        drawGhosts = !drawGhosts;
    
    // update simulation if not paused
    if (!paused)
        particleLife.update();
}


void App::draw(ParticleLife& particleLife) const
{
    BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);

            particleLife.draw(particleTexture.id);
            if (drawGrid)   particleLife.drawGrid();
            if (drawGhosts) particleLife.drawGhosts(particleTexture.id);
            particleLife.drawSoftBorder();

        EndMode2D();

        DrawFPS(10, 10);
    EndDrawing();
}


void App::loadSettings()
{
    auto cleanSpaces = [](std::string& s) { s.erase(std::remove_if(s.begin(), s.end(), [](char c) { return std::isspace(c); }), s.end()); };
    auto cleanCommas = [](std::string& s) { s.erase(std::remove_if(s.begin(), s.end(), [](char c) { return c == ','; }), s.end()); };

    auto valf = [cleanSpaces](std::istream& ss, std::string& str, float& f) {
        if (!std::getline(ss, str, ',')) return false;
        cleanSpaces(str);
        try { f = std::stof(str); }
        catch (const std::invalid_argument& e) { return false; }
        return true;
    }; auto vali = [cleanSpaces](std::istream& ss, std::string& str, int& i) {
        if (!std::getline(ss, str, ',')) return false;
        cleanSpaces(str);
        try { i = std::stoi(str); }
        catch (const std::invalid_argument& e) { return false; }
        return true;
    };
    auto linef = [cleanSpaces, cleanCommas, valf](std::ifstream& file, std::string& line, float& f) {
        if (!std::getline(file, line)) return false;
        cleanSpaces(line), cleanCommas(line);
        try { f = std::stof(line); }
        catch (const std::invalid_argument& e) { return false; }
        return true;
    }; auto linei = [cleanSpaces, cleanCommas](std::ifstream& file, std::string& line, int& i) {
        if (!std::getline(file, line)) return false;
        cleanSpaces(line), cleanCommas(line);
        try { i = std::stoi(line); }
        catch (const std::invalid_argument& e) { return false; }
        return true;
    };
    
    auto invalidArg = [](const std::string& filename, const std::string& value, const std::string type, const std::string dataPoint, int row, int col) {
        std::string msg = filename + ":" + std::to_string(row) + ':' + std::to_string(col) + " " +
        msg += "Invalid " + type + " \"" + value + "\" for " + dataPoint + " data point";
        throw std::invalid_argument(msg);
    };
    auto invalidArgCount = [](const std::string& filename, const std::string dataPoint, std::vector<int> argsNeeded, int argCount, int row, int col) {
        std::string msg = filename + ":" + std::to_string(row) + ':' + std::to_string(col) + " " +
        msg += "Invalid argument count for " + dataPoint + " " +
               "expected " + std::to_string(argsNeeded[0]);
        if (argsNeeded.size() > 1)
            for (int i = 1; i < argsNeeded.size(); i++)
                msg += " or " + std::to_string(argsNeeded[i]);
        msg += " but got " + std::to_string(argCount);
        throw std::invalid_argument(msg);
    };
    
    std::string path = "settings/default/";
    for (const auto& entry : fs::directory_iterator(path)) {
        std::string filename = entry.path().filename().string();
        std::ifstream file(path + entry.path().filename().string());
        std::string line;
        ParticleLife::Settings set;
        int row = 1, col = 1;
        int i, j;

        set.name = filename.substr(0, filename.length() - 4);
        row++;

        if (!linei(file, line, set.types));
        row++;
        if (!linei(file, line, set.size));
        row++;
        if (!linei(file, line, set.count));
        row++;
        if (!linef(file, line, set.innerRadius));
        row++;
        if (!linef(file, line, set.resistance));
        row++;
        if (!linef(file, line, set.step));
        row++;

        // matf(file, line, set.attractions);
        set.attractions = std::vector<std::vector<float>>(set.types, std::vector<float>(set.types, 0));
        for (i = 0; i < set.types; i++) {
            if (!std::getline(file, line)) break;
            std::stringstream ss(line);
            std::string str;
            std::cout << line << std::endl;
            for (j = 0; j < set.types; j++)
                if (!valf(ss, str, set.attractions[i][j])) break;
            if (j < set.types) break;
        } row += i+1; col += j;
        // query i and j for validation

        linei(file, line, set.seed);
        row++;

        if (set.seed == -1) {
            for (i = 0; i < set.count; i++, j = 0) {
                if(!std::getline(file, line)) break;
                std::stringstream ss(line);
                std::string str;
                Particle p;
                if (!vali(ss, str, p.type)) break;
                j++; 
                if (!valf(ss, str, p.pos.x)) break;
                j++;
                if (!valf(ss, str, p.pos.y)) break;
                j++;
                if (valf(ss, str, p.vel.x)) {
                    j++;
                    if (!valf(ss, str, p.vel.y)) break;
                    j++;
                }
                set.particles.emplace_back(p);
            }
        } row += i+1; col += j+1;
        // queury i and j for validation

        file.close();
        defaultSettings.push_back(set);
    }
}

int App::strToUSInt(const std::string& str) const
{
    // validate string size
    const int len = str.length();
    if (len == 0) throw std::invalid_argument("cannot convert empty string");
    
    // validate, store and count digits
    int count = 0;
    std::vector<int> digs;
    for (int i = 0; i < len; i++) {
        if (!std::isdigit(str[i]))
            throw std::invalid_argument("expected digit at s["+std::to_string(i)+"], got '"+str[i]+"'");
        digs.emplace_back(str[i] - '0');
        count++;
    }
    // convert and return unsigned int
    int i = 0;
    int val = 0;
    for (; i < count-2; i++)
        if (digs[i] != 0) val += digs[i] * std::pow(10, count - i - 1);
    val += digs[i++] * 10 + digs[i];
    return val;
}

float App::strToFloat(const std::string& s) const
{
    const int len = s.length(); // validate string size
    if (len == 0) throw std::invalid_argument("invalid float, (len == 0)");

    int iCount = 0, fCount = 0;     // count of integer and fractional digit count
    std::vector<int> idgis, fdigs;  // integer and fractional digits
    bool neg = false;               // negative flag
    int i = 0;                      // char index

    if (s[i] == '-') neg = true, i++; // set neg flag, skip 1st char if negative sign

    // validate, store and count integer digits
    for (; i < len && s[i] != '.'; i++) {
        if (!std::isdigit(s[i]))
            throw std::invalid_argument(
                "Invalid float, expected digit at s["+std::to_string(i)+"], got '"+s[i]+"'"
            );
        idgis.emplace_back(s[i] - '0');
        iCount++;
    }
    if (i < len) { // if has fractional part
        if (++i == len) // skip decimal point, throw if no digits after '.'
            throw std::invalid_argument("invalid float, no digits after '.'");

        // validate, store and count fractional digits
        for (; i < len; i++) {
            if (!std::isdigit(s[i]))
                throw std::invalid_argument(
                    "invalid float, i=" + std::to_string(i) + ", s[i]=" + s[i]
                );
            fdigs.emplace_back(s[i] - '0');
            fCount++;
        }
    }
    // convert and return float
    float val = 0.0f;
    for (i = 0; i < iCount; i++)
        val += idgis[i] * std::pow(10, iCount - i - 1);
    for (i = 0; i < fCount; i++)
        val += fdigs[i] * std::pow(10, -i - 1);
    if (neg) val *= -1;
    return val;
}

void App::testStringConversions() const
{
    auto validate = [this](const std::string& strVal, bool toSucceed, auto conv, auto comp) {
        bool raised;
        std::string msg = strVal + " -> ";
        try { auto val = conv(strVal);
              msg += std::to_string(val);
              msg = toSucceed ? "|PASSED|  " + msg : "|FAILED|  " + msg;
              raised = false;
        } catch (const std::invalid_argument& e) {
            msg = toSucceed ? "|FAILED|  " + msg : "|PASSED|  " + msg;
            msg += e.what();
            raised = true;
        } bool pass = (toSucceed && !raised) || (!toSucceed && raised);
        if (pass) std::cout << msg << std::endl;
        return pass;
    };
    auto validateUSInt = [this, validate](const std::string& strVal, bool toSucceed) {
        return validate(
            strVal, toSucceed,
            [this](const std::string& str){ return strToUSInt(str); },
            [](unsigned int val, const std::string& str){ return val - std::stoul(str); }
        );
    };
    auto validateFloat = [this, validate](const std::string& strVal, bool toSucceed) {
        return validate(
            strVal, toSucceed,
            [this](const std::string& str) { return strToFloat(str); },
            [](float val, const std::string& str) { return val - std::stof(str); }
        );
    };

    std::vector<std::string> usIntValids;
    std::vector<std::string> usIntInvalids;
    std::vector<std::string> usIntFails;

    std::vector<std::string> floatValids;
    std::vector<std::string> floatInvalids;
    std::vector<std::string> floatFails;

    std::cout << "Conversion Tests" << std::endl;

    for (int i = 1; i <= 10; i++) {
        for (int j = 1; j <= 10; j++) {
            for (int k = 1; k <= 10; k++) {
                std::string str = std::to_string(i*i*j*j*k*k);
                std::string rev = str;
                std::reverse(rev.begin(), rev.end());
                floatValids.emplace_back(str+'.'+rev); std::cout << floatValids.back() << std::endl;
                usIntValids.emplace_back(str);
                usIntValids.emplace_back(rev);
                floatValids.emplace_back(str+'.'+str); std::cout << floatValids.back() << std::endl;
                floatValids.emplace_back(rev+'.'+rev); std::cout << floatValids.back() << std::endl;
                floatValids.emplace_back(rev+'.'+rev); std::cout << floatValids.back() << std::endl;
            }
        }
    }
    
    for (std::string& str : floatValids) {
        std::cout << "| strToFloat(" << str << ") ";
        if (validateFloat(str, true)) continue;
        floatFails.emplace_back(str);
    } for (std::string& str : usIntValids) {
        std::cout << "| strToUSInt(" << str << ") ";
        if (validateUSInt(str, true)) continue;
        usIntFails.emplace_back(str);
    }

    
    for (std::string& fail : usIntFails) {
        std::cout << 
        validateUSInt(fail, true);
    } for (std::string& fail : floatFails) {
        validateFloat(fail, true);
    }

    std::cout << "Conversion Test Results" << std::endl << std::endl;

    std::cout << "|strToFloat(const std::string& s) -> float           " <<
                 floatValids.size() - floatFails.size() << " / " << floatValids.size() << std::endl;

    std::cout << "|strToUSInt(const std::string& s) -> unsigned int    " <<
                 usIntValids.size() - usIntFails.size() << " / " << usIntValids.size() << std::endl << std::endl;
                 
}