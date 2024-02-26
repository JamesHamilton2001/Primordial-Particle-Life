#include "Particle.hpp"

#include <raylib.h>
#include <iostream>
#include <iomanip>


Particle::Particle() :
    type(0),
    pos(Vector2 { 0, 0 }),
    vel(Vector2 { 0, 0 })
{}

Particle::Particle(int type, Vector2 pos, Vector2 vel) :
    type(type),
    pos(pos),
    vel(vel)
{}

Particle::Particle(const Particle& p) :
    type(p.type),
    pos(p.pos),
    vel(p.vel)
{}

std::ostream& operator<<(std::ostream& os, const Particle& p)
{
    os << " "   << p.type << " : " << p.pos.x << ", " << p.pos.y;
    return os;
}
