#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <raylib.h>
#include <iostream>
#include <string>


class Particle
{
 public:

    Particle();
    Particle(int type, Vector2 pos, Vector2 vel);
    Particle(const Particle& p);

    int type;
    Vector2 pos;
    Vector2 vel;

    friend std::ostream& operator <<(std::ostream& os, const Particle& p);

};


#endif