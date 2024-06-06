#include "Particle.hpp"

#include <raylib.h>
#include <iostream>
#include <iomanip>



Particle::Particle(unsigned int type, Vector2 pos, Vector2 vel) :
    type(type),
    pos(pos),
    vel(vel)
{}

Particle::Particle(unsigned int type, Vector2 pos) :
    type(type),
    pos(pos),
    vel(Vector2 { 0, 0 })
{}

Particle::Particle(unsigned int type, float x, float y, float xv, float yv) :
    Particle(type, Vector2 { x, y }, Vector2 { xv, yv })
{}

Particle::Particle(unsigned int type, float x, float y) :
    Particle(type, Vector2 { x, y })
{}

Particle::Particle() :
    type(0),
    pos(Vector2 { 0, 0 }),
    vel(Vector2 { 0, 0 })
{}

Particle::Particle(const Particle& p) :
    type(p.type),
    pos(p.pos),
    vel(p.vel)
{}


ostream& operator<<(ostream& os, const Particle& p)
{
    return os << p.type <<", ("<< p.pos.x <<", "<< p.pos.y <<"), ("<< p.vel.x <<", "<< p.vel.y <<")";
}
