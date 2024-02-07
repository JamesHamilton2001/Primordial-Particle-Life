#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <raylib.h>
#include <iostream>
#include <string>


class Particle
{
 public:

    int type;
    Vector2 pos;
    Vector2 vel;

    friend std::ostream& operator<<(std::ostream& os, const Particle& p);

 private:
    
    std::string fmtVecStr(Vector2 vec, int iDigs, int fDigs) const;
    std::string fmtFloatStr(float val, int iDigs, int fDigs) const;

};


#endif