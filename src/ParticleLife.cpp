#include "ParticleLife.hpp"

#include <raylib.h>
#include <vector>
#include <cmath>


ParticleLife::ParticleLife(int types, int size, int count, float resistance, float innerRadius, float step, std::vector<std::vector<float>> attractions, int seed) :
    types       (types),
    size        (size),
    bounds      (nextafterf(2 * size, 0.0f)),
    count       (count),
    resistance  (resistance),
    innerRadius (innerRadius),
    step        (step),
    attractions (attractions)
{
    particles.resize(count);

    SetRandomSeed(seed);

    for (Particle& p : particles) {
        p.type = GetRandomValue(0, types-1);
        p.pos = Vector2 { GetRandomValue(0, 100*bounds) / 100.0f, GetRandomValue(0, 100*bounds) / 100.0f };
        p.vel = Vector2 { 0, 0 };
    }
}

void ParticleLife::update()
{
    for (int i = 0; i < count; i++) {
        Particle& p1 = particles[i];

        for (int j = 0; j < count; j++) {
            if (i == j) continue;

            Particle& p2 = particles[j];

            const float dx = p2.pos.x - p1.pos.x;
            const float dy = p2.pos.y - p1.pos.y;
            const float sqDist = dx*dx + dy*dy;

            if (sqDist <= 4.0f) {
                float dist = sqrt(sqDist);

                const float coef = (dist <= innerRadius)
                    ? 1.0f - innerRadius / dist
                    : attractions[p1.type][p2.type] * (dist - innerRadius) / 2.0f;

                p1.vel.x += coef * (dx / dist);
                p1.vel.y += coef * (dy / dist);

            }
        }
    }

    const float invResistance = 1.0f - resistance;

    for (Particle& p : particles) {

        p.vel.x *= invResistance;
        p.vel.y *= invResistance;

        p.pos.x += step * p.vel.x;
        p.pos.y += step * p.vel.y;

        if (p.pos.x < 0.0f)   p.pos.x = 0.0f,   p.vel.x *= -1.0f;
        if (p.pos.x > bounds) p.pos.x = bounds, p.vel.x *= -1.0f;
        if (p.pos.y < 0.0f)   p.pos.y = 0.0f,   p.vel.y *= -1.0f;
        if (p.pos.y > bounds) p.pos.y = bounds, p.vel.y *= -1.0f;

    }
}
