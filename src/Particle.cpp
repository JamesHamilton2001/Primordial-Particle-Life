#include "Particle.hpp"

#include <raylib.h>
#include <iostream>
#include <iomanip>


Particle::Particle() :
    type(0),
    pos(Vector2 { 0, 0 }),
    vel(Vector2 { 0, 0 })
{}


std::ostream& operator<<(std::ostream& os, const Particle& p)
{
    os << "| Type: " << p.type <<
         " | Pos: " << p.fmtVecStr(p.pos, 4, 2) <<
         " | Vel: " << p.fmtVecStr(p.vel, 3, 3) << " |";
    return os;
}


std::string Particle::fmtVecStr(Vector2 vec, int iDigs, int fDigs) const
{
    return fmtFloatStr(vec.x, iDigs, fDigs) + ", " + fmtFloatStr(vec.y, iDigs, fDigs);
}


std::string Particle::fmtFloatStr(float val, int iDigs, int fDigs) const
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(fDigs) << std::abs(val); // Use std::abs to get the absolute value

    std::string str = ss.str();

    int point = str.find('.');
    std::string iStr = str.substr(0, point);
    std::string fStr = str.substr(point);

    while (static_cast<int>(iStr.length()) < iDigs)
        iStr = "0" + iStr;

    while (static_cast<int>(fStr.length()) < fDigs+1)
        fStr += "0";

    return (val < 0 ? "-" : " ") + iStr + fStr;
}