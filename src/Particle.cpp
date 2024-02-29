#include "Particle.hpp"

#include <raylib.h>
#include <iostream>
#include <iomanip>


// RED, BLUE, YELLOW, PURPLE, GREEN, ORANGE, PINK, BEIGE, RAYWHITE
const unsigned char Particle::R[9] = { 255, 50,  255, 255, 0,   255, 255, 210, 255 };
const unsigned char Particle::G[9] = { 50,  100, 255, 127, 255, 127, 200, 170, 255 };
const unsigned char Particle::B[9] = { 50,  255, 0,   255, 50,  0,   200, 130, 255 };


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
    return os << p.type <<" : "<< p.pos.x <<", "<< p.pos.y <<" : "<< p.vel.x <<", "<< p.vel.y;
}
