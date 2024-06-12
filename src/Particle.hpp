#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <raylib.h>
#include <rlgl.h>
#include <fstream>
#include <iostream>


using namespace std;


class Particle
{
 public:

    Particle(unsigned int type, Vector2 pos, Vector2 vel);
    Particle(unsigned int type, Vector2 pos);
    Particle(unsigned int type, float x, float y, float xv, float xy);
    Particle(unsigned int type, float x, float y);
    Particle(const Particle& p);
    Particle();

    unsigned int type;
    Vector2 pos;
    Vector2 vel;

    friend ostream& operator <<(ostream& os, const Particle& p);

};


#endif
