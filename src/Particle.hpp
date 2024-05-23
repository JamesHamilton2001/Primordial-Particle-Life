#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <raylib.h>
#include <rlgl.h>
#include <iostream>
#include <string>

using namespace std;


class Particle
{
 public:

    static const unsigned char R[9], G[9], B[9];

    Particle(int type, Vector2 pos, Vector2 vel);
    Particle(int type, Vector2 pos);
    Particle(int type, float x, float y, float xv, float xy);
    Particle(int type, float x, float y);
    Particle();
    Particle(const Particle& p);

    int type;
    Vector2 pos;
    Vector2 vel;
    
    inline void draw() const        // requires: texture, RL_QUADS   
    {
        rlColor4ub(R[type], G[type], B[type], 255);
        rlNormal3f(0.0f, 0.0f, 1.0f);
        rlTexCoord2f(0.0f, 0.0f); rlVertex2f(pos.x-0.05f, pos.y-0.05f); // top left
        rlTexCoord2f(0.0f, 1.0f); rlVertex2f(pos.x-0.05f, pos.y+0.05f); // bottom left
        rlTexCoord2f(1.0f, 1.0f); rlVertex2f(pos.x+0.05f, pos.y+0.05f); // bottom right
        rlTexCoord2f(1.0f, 0.0f); rlVertex2f(pos.x+0.05f, pos.y-0.05f); // top right
    }

    friend ostream& operator <<(ostream& os, const Particle& p);

};


#endif
