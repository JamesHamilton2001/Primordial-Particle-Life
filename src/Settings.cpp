#include "Settings.hpp"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <unordered_map>
#include <functional>



Settings::Settings() :
    name("DEFAULT_HARDCODED_SETTINGS"),
    types(PARTICLELIFE_MIN_TYPES),
    size(PARTICLELIFE_MIN_GRID_SIZE),
    count(15),
    innerRadius(0.5f),
    resistance(0.001f),
    step(0.001f),
    attractions(vector<vector<float>>( {{  0.005,  0.002, -0.002 },
                                        { -0.002,  0.005,  0.002 },
                                        {  0.002, -0.002,  0.005 }} )),
    seed(-1),
    typeRatio(vector<int>( { 1, 1, 1 } )),
    particles(vector<Particle>( { Particle(0, 2, 4), Particle(1, 2, 1), Particle(2, 3, 1),
                                  Particle(0, 5, 7), Particle(1, 2, 1), Particle(2, 4, 4),
                                  Particle(0, 4, 5), Particle(1, 6, 7), Particle(2, 5, 2),
                                  Particle(0, 6, 6), Particle(1, 8, 1), Particle(2, 6, 6),
                                  Particle(0, 6, 7), Particle(1, 7, 1), Particle(2, 3, 1), } ))
{}

Settings::Settings(const filesystem::directory_entry& dirEntry)
{
    ifstream file(dirEntry.path());
    string line;
    int row = 1;

    const auto clean = [](string& s) {
        s.erase(remove_if(s.begin(), s.end(), [](char c) { return isspace(c); }), s.end());
    };
    const auto throwBadRead = [&row](const string& dataPoint) {
        throw invalid_argument("line:" + to_string(row) + " Unreadable " + dataPoint + " data");
    };

    // get name from filename
    string fileName = dirEntry.path().filename().string();
    name = fileName.substr(0, fileName.length() - 4);

    // get types
    if (!getline(file, line)) throwBadRead("types");
    clean(line);
    types = stoi(line);
    row++;

    // get size
    if (!getline(file, line)) throwBadRead("size");
    clean(line);
    size = stoi(line);
    row++;

    //get count
    if (!getline(file, line)) throwBadRead("count");
    clean(line);
    count = stoi(line);
    row++;

    // get innerRadius
    if (!getline(file, line)) throwBadRead("innerRadius");
    clean(line);
    innerRadius = stof(line);
    row++;

    // get resistance
    if (!getline(file, line)) throwBadRead("resistance");
    clean(line);
    resistance = stof(line);
    row++;

    // get step
    if (!getline(file, line)) throwBadRead("step");
    clean(line);
    step = stof(line);
    row++;

    // get attractions
    attractions = vector<vector<float>>(types, vector<float>(types, 0));
    for (int i = 0; i < types; i++) {
        if (!getline(file, line)) throwBadRead("attractions");
        clean(line);
        stringstream ss(line);
        string str;
        for (int j = 0; j < types; j++) {
            if (!getline(ss, str, ',')) throwBadRead("attractions");
            clean(str);
            attractions[i][j] = stof(str);
        }
        row++;
    }

    // get seed
    if (!getline(file, line)) throwBadRead("seed");
    clean(line);
    seed = stoi(line);
    row++;

    // if seed is -1 then get particles
    if (seed == -1) {
        for (int i = 0; i < count; i++) {
            if (!getline(file, line)) throwBadRead("particle");
            clean(line);
            stringstream ss(line);
            string str;
            Particle p;
            if (!getline(ss, str, ',')) throwBadRead("particle type");
            clean(str);
            p.type = stoi(str);
            if (!getline(ss, str, ',')) throwBadRead("particle pos.x");
            clean(str);
            p.pos.x = stof(str);
            if (!getline(ss, str, ',')) throwBadRead("particle pos.y");
            clean(str);
            p.pos.y = stof(str);
            if (!getline(ss, str, ',')) throwBadRead("particle vel.x");
            clean(str);
            p.vel.x = stof(str);
            if (!getline(ss, str, ',')) throwBadRead("particle vel.y");
            clean(str);
            p.vel.y = stof(str);
            particles.emplace_back(p);
            row++;
        }
        // TODO: calculate simplified type ratio
        typeRatio = vector<int>(types, 0);
        for (const Particle& p : particles) typeRatio[p.type]++;
    }
    // otherwise get particle type ratio
    else { 
        typeRatio = vector<int>(types, 0);
        if (!getline(file, line)) throwBadRead("typeRatio");
        clean(line);
        stringstream ss(line);
        string str;
        for (int i = 0; i < types; i++) {
            if (!getline(ss, str, ',')) throwBadRead("typeRatio");
            clean(str);
            typeRatio[i] = stoi(str);
        }
        particles = vector<Particle>(count);
    }

    file.close();
}

void Settings::save() const
{
    const string filePath = PARTICLELIFE_CUSTOM_SETTINGS_DIR + name + ".json";
    ofstream f (filePath);
    if (!f.is_open()) {
        cerr << "Failed to create/open \""+filePath+"\" for writing settings." << endl;
        return;
    }
    const string M = "    ";

    f << "{\n";

        f << M << "\"SimulationSettings\":\n"
          << M << "{\n";

            f << M+M << "\"name\": "        << "\""+name+"\"" << ",\n"
              << M+M << "\"types\": "       << types << ",\n"
              << M+M << "\"size\": "        << size << ",\n"
              << M+M << "\"count\": "       << count << ",\n"
              << M+M << "\"innerRadius\": " << innerRadius << ",\n"
              << M+M << "\"resistance\": "  << resistance << ",\n"
              << M+M << "\"step\": "        << step  << ",\n";

            f << M+M << "\"attractions\": [ ";
            for (int i = 0; i < types; i++) {
                if (i != 0) f << M+M << "                 ";
                f << '[' << attractions[i][0];
                for (int j = 1; j < types; j++) {
                    f << ", " << attractions[i][j];
                } f << ']';
                if (i == types-1) f << " ]";
                f << ",\n";
            }

            f << M+M << "\"seed\": " << seed << ",\n";

            f << M+M << "\"typeRatio\": [" << typeRatio[0];
            for (int i = 1; i < types; i++) {
                f << ", " << typeRatio[i];
            } f << " ],\n";

            f << M+M << "\"particles\": [ ";
            if (!particles.empty()) {
                f << particles[0];
                for (int i = 1; i < count; i++)
                    f << ",\n" << M+M << "               " << particles[i];
            } f << " ]\n";

        f << M << "}\n";

    f << "}" << endl;

    f.close();
}

ostream& operator <<(ostream& os, const Settings& settings)
{
    os << "| name | " << settings.name << endl
       << "| types | " << settings.types << endl
       << "| size | " << settings.size << endl
       << "| count | " << settings.count << endl
       << "| innerRadius | " << settings.innerRadius << endl
       << "| resistance | " << settings.resistance << endl
       << "| step | " << settings.step << endl;

    os << "| attractions | " << endl;
    for (int i = 0; i < settings.types; i++) {
        os << "| | " << settings.attractions[i][0];
        for (int j = 1; j < settings.types; j++)
            os << ", " << settings.attractions[i][j];
        os << endl;
    }
    os << "| seed | " << settings.seed << endl;

    os << "| typeRatio | " << settings.typeRatio[0];
    for (int i = 1; i < settings.types; i++)
        os << ", " << settings.typeRatio[i];
    cout << endl;

    os << "| particles | ";
    vector<int> typeCounts(settings.types, 0);
    for (const Particle& p : settings.particles) typeCounts[p.type]++;
    cout << typeCounts[0];
    for (int i = 1; i < settings.types; i++)
        cout << ", " << typeCounts[i];
    cout << endl;
    if (settings.particles.size() <= 32)
        for (const Particle& p : settings.particles)
            os << "| | " << p << endl;
    else os << "| | lots..." << endl;

    return os;
}

void Settings::generateParticleData()
{
    // dont overwrite for preloading particles, recalculate ratio
    if (seed == -1) {
        typeRatio = vector<int>(types, 0);
        for (Particle& p : particles)
            typeRatio[p.type]++;
        return;
    }

    SetRandomSeed(seed);
    particles.clear();
    int ct = 0;
    while (ct < count) {
        for (int t = 0; t < types; t++) {
            for (int i = 0; i < typeRatio[t] && ct < count; i++) {
                particles.emplace_back(Particle(
                    t,
                    GetRandomValue(0, static_cast<int>(2*size)) + GetRandomValue(0, 1000) / 1000.0f,
                    GetRandomValue(0, static_cast<int>(2*size)) + GetRandomValue(0, 1000) / 1000.0f
                ));
                ct++;
            }
        }
    }
}

/* 
Settings::Settings(string filePath)
{
    ifstream file(filePath);
    if (!file.is_open())
        throw invalid_argument("Failed to open \""+filePath+"\" for reading settings.");

    const unsigned int TOTAL_ATTRIBUTES = 11U;

    string line;
    long long unsigned int lno = 0LLU;
    long long unsigned int i = 0LLU;
    unsigned int attributesRead = 0U;

    parseChar(file, line, lno, i, '{');
    string str = getParseString(file, line, lno, i);
    if (str != "SimulationSettings")
        throw invalid_argument("line:" + to_string(lno) + " SimulationSettings declaration not found");
    parseChar(file, line, lno, i, ':');
    parseChar(file, line, lno, i, '{');

    while (attributesRead < TOTAL_ATTRIBUTES) {

        const string attributeName = getParseString(file, line, lno, i);
        if (attributteParsers.find(attributeName) == attributteParsers.end())
            throw invalid_argument("line:" + to_string(lno) + " Unreadable attribute name: " + attributeName);
        (this->*attributteParsers.at(attributeName))(file, line, lno, i);
        attributesRead++;

        cout << "line:" << lno << " reading: " << attributeName << endl;
    }
    charIsNext(file, line, lno, i, '}');
    charIsNext(file, line, lno, i, '}');

    cout << *this << endl;
}
 */

Settings::Settings(string filePath)
{
    JsonParser parser(filePath, *this);
    parser.parseIntoSettings();
}

Settings::JsonParser::JsonParser(string filePath, Settings settings) :
    settings(settings)
{
    file.open(filePath);
    if (!file.is_open())
        throw invalid_argument("Failed to open \""+filePath+"\" for reading settings.");

    string rawLine;
    bool quoted = false;
    row = 0;
    while (getline(file, rawLine)) {
        col = 0;
        len = rawLine.length();
        string cleanBuffer = "";
        for (char queryChar = rawLine[col]; col < len; queryChar = rawLine[++col]) {
            if (queryChar == '"') {
                if (col > 0 && rawLine[col-1] == '\\') continue;
                quoted = !quoted;
            } else if (queryChar == ' ' && !quoted) continue;
            cleanBuffer += queryChar;
        } if (!cleanBuffer.empty()) {
            cout << cleanBuffer << endl;
            lines.emplace_back(cleanBuffer);
        }
    }

    row = 0LLU;
    col = 0LLU;
    len = lines[row].size();
    count = lines.size();

    prev = '\0';
    curr = lines[row][col];
}

void Settings::JsonParser::parseIntoSettings()
{
    string attributeName;

    if (curr != '{')
        throw invalid_argument(posString() + "Expected '{' to begin file");
    step();
    attributeName = parseGetKey();
    if (attributeName != "SimulationSettings")
        throw invalid_argument(posString() + "Expected \"SimulationSettings\" declaration");
    if (curr != '{')
        throw invalid_argument(posString() + "Expected '{' to begin settings, got '"+curr+"' instead");
    step();

    // NAME
    attributeName = parseGetKey();
    settings.name = parseGetString();
    if (curr != ',')
        throw invalid_argument(posString() + "Expected ',' after declaration, got '"+curr+"' instead");
    cout << "name: " << settings.name << endl;
    step();

    // TYPES
    attributeName = parseGetKey();
    settings.types = parseGetInt();
    if (curr != ',')
        throw invalid_argument(posString() + "Expected ',' after declaration, got '"+curr+"' instead");
    cout << "types: " << settings.types << endl;
    step();

    // SIZE
    attributeName = parseGetKey();
    settings.size = parseGetInt();
    if (curr != ',')
        throw invalid_argument(posString() + "Expected ',' after declaration, got '"+curr+"' instead");
    cout << "size: " << settings.size << endl;
    step();

    // COUNT
    attributeName = parseGetKey();
    settings.count = parseGetInt();
    if (curr != ',')
        throw invalid_argument(posString() + "Expected ',' after declaration, got '"+curr+"' instead");
    cout << "count: " << settings.count << endl;
    step();

    // INNER RADIUS
    attributeName = parseGetKey();
    settings.innerRadius = parseGetFloat();
    if (curr != ',')
        throw invalid_argument(posString() + "Expected ',' after declaration, got '"+curr+"' instead");
    cout << "innerRadius: " << settings.innerRadius << endl;
    step();

    // RESISTANCE
    attributeName = parseGetKey();
    settings.resistance = parseGetFloat();
    if (curr != ',')
        throw invalid_argument(posString() + "Expected ',' after declaration, got '"+curr+"' instead");
    cout << "resistance: " << settings.resistance << endl;
    step();

    // STEP
    attributeName = parseGetKey();
    settings.step = parseGetFloat();
    if (curr != ',')
        throw invalid_argument(posString() + "Expected ',' after declaration, got '"+curr+"' instead");
    cout << "step: " << settings.step << endl;
    step();

    cout << endl << endl << settings << endl;

    // if (curr != '}')
        // throw invalid_argument(posString() + "Expected '}' to end file");

}

Settings::JsonParser::~JsonParser()
{
    file.close();
}

string Settings::JsonParser::parseGetKey()
{
    return parseGetString(true);
}

string Settings::JsonParser::parseGetString()
{
    return parseGetString(false);
}

string Settings::JsonParser::parseGetString(bool isKey)
{
    const long long unsigned int startRow = row;

    if (curr != '"')
        throw invalid_argument(posString()+"Expected '\"' to begin string, got '"+curr+"' instead");

    string str = "";
    while (true)
    {
        if (!step()) throw invalid_argument(posString() + "File cannot end with string");
        if (startRow != row) throw invalid_argument(posString() + "String can only span one line");
        if (curr != '"' && (str += curr, true)) continue;

        if (!step()) throw invalid_argument(posString() + "File cannot end with string");
        if (isKey) {
            if (curr != ':') throw invalid_argument(posString() + "Expected ':' after key");
            if (!step()) throw invalid_argument(posString() + "File cannot end with key");
        } else if (curr != ',' && curr != '}' && curr != ']')
            throw invalid_argument(posString() + "Expected ',' or '}' or ']' after string, got '"+curr+"' instead");
        return str;
    }
}

int Settings::JsonParser::parseGetInt()
{
    // NOTE: remember true is 1, false is 0, and 0 is falsey, 1 and above is truthy
    const long long unsigned int R = row;
    const long long unsigned int C = col;
    const bool sign = curr == '-';

    if (sign && !step())
        throw invalid_argument(posString()+"File cannot end with integer value");

    unsigned int digs = 0U;
    while (true)
    {
        if (row != R) throw invalid_argument(posString()+"Integer can only span one line");
        if (!isdigit(curr)) throw invalid_argument(posString()+"Invalid char '"+curr+"' in integer");
        if (!step()) throw invalid_argument(posString()+"File cannot end with integer value");
        digs++;
        if (curr == ',' || curr == '}' || curr == ']') {
            if (digs) return stoi(lines[R].substr(C, C+sign+digs));
            throw invalid_argument(posString()+"No numeric value given for integer");
        }
    }
    throw logic_error(posString()+"Settings::JsonParser::parseGetInt() exited loop without returning");
}

float Settings::JsonParser::parseGetFloat()
{
    // NOTE: remember true is 1, false is 0, and 0 is falsey, 1 and above is truthy
    const long long unsigned int R = row;
    const long long unsigned int C = col;

    bool point = false;
    unsigned int digits = 0U;
    unsigned int chars = curr == '-';
    if (chars) step();
    while(row == R && step())
    {
        chars++;
        if (!point && digits && prev == '.') {
            point = true;
            digits = 0U;
        } else if (isdigit(prev)) {
            digits++;
            if (curr == ',' || curr == '}' || curr == ']') {
                if (digits) return stof(lines[R].substr(C, C+chars));
                break;
            }
        }
    } throw invalid_argument(posString()+"1Invalid char '"+curr+"' in float");
}

ostream &operator<<(ostream &os, const Settings::JsonParser &parser)
{
    cout << "JsonParser: { "
         << "row: " << parser.row << ", col: " << parser.col
         << ", len: " << parser.len << ", count: " << parser.count
         << ", chars: ";
    if (parser.prev != '\0') cout << '\'' << parser.prev << "\' => ";
    else cout << "NULL => ";
    if (parser.curr != '\0') cout << '\'' << parser.curr << "\' => ";
    else cout << "NULL => ";
    cout << "}";
    return os;
}

bool Settings::JsonParser::step()
{
    col++;
    prev = curr;
    if (col >= len) {
        row++;
        if (row == count) {
            len = 0;
            curr = '\0';
            return false;
        } col = 0;
        len = lines[row].size();
    } curr = lines[row][col];
    return true;
}

string Settings::JsonParser::posString(int r, int c)
{
    return "line:" + to_string(r+1) + ',' + to_string(c+1) + "  ";
}

string Settings::JsonParser::posString()
{
    return posString(row, col);
}