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

/*
Settings::Settings(string filePath)
{
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Failed to open \""+filePath+"\" for reading settings." << endl;
        return;
    }

    const function<bool(string&)> hasdigit = [](string& s) { return any_of(s.begin(), s.end(), [](unsigned char c) { return isdigit(c); }); };

    const function<void(string&, string&)> readOneLineString = [](string& line, string& dest) {
        int len = static_cast<int>(line.size());
        int i, j;
        for (i = 0; line[i] != ':' && i < len; i++);
        for (; line[i] != '\"' && i < len-1; i++);
        for (j = i+1; line[j] != '\"' && j < len; j++);
        dest = line.substr(i+1, j-i-1);
    };

    const function<void(string&, int*)> readOneLineInt = [this](string& line, int* ptr) {
        int len = static_cast<int>(line.size());
        int i, j; 
        for (i = 0; line[i] != ':' && i < len; i++);
        for (; !isdigit(line[i]) && line[i] != '-' && i < len-1; i++);
        for (j = i+1; line[j] != ',' && line[j] != ' ' && j < len; j++);
        *ptr = stoi(line.substr(i, j-i));
    };

    const function<void(string&, float*)> readOneLineFloat = [this](string& line, float* ptr) {
        int len = static_cast<int>(line.size());
        int i, j;
        for (i = 0; line[i] != ':' && i < len; i++);
        for (; !isdigit(line[i]) && line[i]!='-' && line[i]!='+' && line[i] != '.' && i < len-1; i++);
        for (j = i+1; line[j] != ',' && line[j] != ' ' && j < len; j++);
        *ptr = stof(line.substr(i, j-i));
    };

    const function<void(string&, vector<int>&)> readIntVector = [this, &hasdigit, &file](string& line, vector<int>& dest) {
        dest = vector<int>();
        bool opened = false, closed = false;
        while (!opened) {
            opened = line.find('[') != string::npos;
            if (!opened && !getline(file, line)) break;
        }
        while (opened && !closed) {
            closed = line.find(']') != string::npos;
            if (!hasdigit(line)) {
                if (!getline(file, line)) break;
                else continue;
            };
            int len = static_cast<int>(line.size());
            int i = 0, j, k;
            while (i < len) {
                for (j = i+1; !isdigit(line[j]) && line[j] != '-' && line[j] != '+' && j < len-1; j++);
                for (k = j+1; line[k] != ',' && line[k] != ' ' && k < len; k++);
                string intString = line.substr(j, k-j);
                if (hasdigit(intString))
                    dest.emplace_back(stoi(intString));
                else if (!getline(file, line))
                    break;
                i = k;
            }
            if (!closed && !getline(file, line)) break;
        }
    };

    const function<void(string&, vector<vector<float>>&)> readFloatMatrix = [this, &file](string& line, vector<vector<float>>& dest) {
        dest = vector<vector<float>>();
        int i, len, opens = 0, closes = 0;

        while (opens == 0) {
            for (i = 0, len = static_cast<int>(line.size()); i < len; i++) {
                if (line[i] == '[') {
                    opens++;
                    break;
                }
            } if (opens == 0 && !getline(file, line)) break;
        } i++;
        line = line.substr(i, len-i);

        while (opens != closes) {
            i = 0;
            string floatString = "";
            while (i < static_cast<int>(line.size())) {
                unsigned char c = line[i];
                if (c=='-' || c=='+' || c=='.' || isdigit(c)) {
                    floatString += c;
                } else {
                    if (!floatString.empty()) {
                        dest.back().emplace_back(stof(floatString));
                        floatString = "";
                    } if (c == '[') {
                        opens++;
                        dest.emplace_back(vector<float>());
                    } else if (c == ']') closes++;
                } i++;
            } if (opens != closes && !getline(file, line)) break;
        }
    };

    const unordered_map<string, function<void(string&)>> readAttribute = {
        { "name", [this, &readOneLineString](string& line)        { readOneLineString(line, name); } },
        { "types", [this, &readOneLineInt](string& line)          { readOneLineInt(line, &types); }},
        { "size", [this, &readOneLineInt](string& line)           { readOneLineInt(line, &size); }},
        { "count", [this, &readOneLineInt](string& line)          { readOneLineInt(line, &count); }},
        { "innerRadius", [this, &readOneLineFloat](string& line)  { readOneLineFloat(line, &innerRadius); }},
        { "resistance", [this, &readOneLineFloat](string& line)   { readOneLineFloat(line, &resistance); }},
        { "step", [this, &readOneLineFloat](string& line)         { readOneLineFloat(line, &step); }},
        { "attractions", [this, &readFloatMatrix](string& line)   { readFloatMatrix(line, attractions); }},
        { "seed", [this, &readOneLineInt](string& line)           { readOneLineInt(line, &seed); }},
        { "typeRatio", [this, &readIntVector](string& line)       { readIntVector(line, typeRatio); }},
        { "particles", [this, &readOneLineFloat](string& line)    {  }},
    };

    string line;
    long long unsigned int i, j;
    while (getline(file, line)) {

        // get attribute name and starting index of value
        for (i = 1; line[i-1] != '\"' && i < line.size()-1; i++);
        for (j = i+1; line[j] != '\"' && j < line.size(); j++);
        string attributeName = line.substr(i, j-i);

        if (readAttribute.find(attributeName) == readAttribute.end()) {
            cerr << "Unreadable attribute name: " << attributeName << endl;
            continue;
        }
        cout << "Reading attribute: " << attributeName << endl;
        readAttribute.at(attributeName)(line);
    }

    file.close();

    for (int tr : typeRatio) cout << tr << " ";
    cout << endl;

    cout << *this << endl;
}
*/

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

    while (getNextString(file, line, lno, i) != "SimulationSettings");

    while (attributesRead < TOTAL_ATTRIBUTES) {
        if (!getline(file, line))
            throw invalid_argument("line:" + to_string(lno) + " end of file reached before all attributes read");
        lno++;
        if (line.empty()) continue;

        const string attributeName = getNextString(file, line, lno, i);
        if (attributteParsers.find(attributeName) == attributteParsers.end())
            throw invalid_argument("line:" + to_string(lno) + " Unreadable attribute name: " + attributeName);
        (this->*attributteParsers.at(attributeName))(file, line, lno, i);
        attributesRead++;

        cout << "line:" << lno << " reading: " << attributeName << endl;
    }

    cout << *this << endl;
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

const string Settings::getNextString(ifstream& file, string& line, unsigned long long int& lno, unsigned long long int& i) const
{
    while (true)
    {
        size_t qStart = line.find('"');
        if (qStart != string::npos) {
            size_t qEnd = line.find('"', qStart+1);

            if (qEnd == string::npos)
                throw invalid_argument("line:" + to_string(lno) + " string not closed");

            string str = line.substr(qStart+1, qEnd-qStart-1);
            line = line.substr(qEnd+1);
            return str;
        }
        if (!getline(file, line))
            throw invalid_argument("line:" + to_string(lno) + " end of file reached before string read");
        lno++;
    }
}


const int Settings::getNextInt(ifstream& file, string& line, unsigned long long int& lno, unsigned long long int& i) const
{
    while (true)
    {
        for (long long unsigned int i = 0ULL; i < line.size(); i++) {
            char c1 = line[i];

            if (isdigit(c1) || c1 == '-' || c1 == '+') {
                for (long long unsigned int j = i+1; j < line.size(); j++) {
                    char c2 = line[j];

                    if (isdigit(c2)) continue;
                    switch (c2) {
                        case ',': case ']': case '}': case ' ':
                            return stoi(line.substr(i, j-i));
                        default:
                            throw invalid_argument("line:" +to_string(lno)+ " invalid char following int");
                    }
                }
            }
        }
        if (!getline(file, line))
            throw invalid_argument("line:" + to_string(lno) + " end of file reached before int read");
        lno++;
    }
}

const float Settings::getNextFloat(ifstream& file, string& line, unsigned long long int& lno, unsigned long long int& i) const
{
    return 0.0f;
}


void Settings::parseName(ifstream& file, string& line, unsigned long long int& lno, unsigned long long int& i)
{
    name = getNextString(file, line, lno, i);
}

void Settings::parseTypes(ifstream& file, string& line, unsigned long long int& lno, unsigned long long int& i)
{
    types = getNextInt(file, line, lno, i);
}

void Settings::parseSize(ifstream& file, string& line, unsigned long long int& lno, unsigned long long int& i)
{
    size = getNextInt(file, line, lno, i);
}

void Settings::parseCount(ifstream& file, string& line, unsigned long long int& lno, unsigned long long int& i)
{
    count = getNextInt(file, line, lno, i);
}

void Settings::parseInnerRadius(ifstream& file, string& line, unsigned long long int& lno, unsigned long long int& i)
{
    innerRadius = getNextFloat(file, line, lno, i);
}

void Settings::parseResistance(ifstream& file, string& line, unsigned long long int& lno, unsigned long long int& i)
{
    resistance = getNextFloat(file, line, lno, i);
}

void Settings::parseStep(ifstream& file, string& line, unsigned long long int& lno, unsigned long long int& i)
{
    step = getNextFloat(file, line, lno, i);
}

void Settings::parseAttractions(ifstream& file, string& line, unsigned long long int& lno, unsigned long long int& i)
{
    attractions = vector<vector<float>>();
}

void Settings::parseSeed(ifstream& file, string& line, unsigned long long int& lno, unsigned long long int& i)
{
    seed = getNextInt(file, line, lno, i);
}

void Settings::parseTypeRatio(ifstream& file, string& line, unsigned long long int& lno, unsigned long long int& i)
{
    typeRatio = vector<int>();
}

void Settings::parseParticles(ifstream& file, string& line, unsigned long long int& lno, unsigned long long int& i)
{
    particles = vector<Particle>();
}