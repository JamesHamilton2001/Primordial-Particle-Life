#include "ParticleLife.hpp"

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <vector>
#include <iostream>


void ParticleLife::init(int typeCount, int particleCount, float particleInnerRadius, float resistance, float step, int gridSize)
{
    // simulation settings
    this->typeCount = typeCount;
    this->count = particleCount;
    this->innerRadius = particleInnerRadius;
    this->resistance = resistance;
    this->step = step;
    this->bounds = 2.0f * gridSize;

    // assign colours
    colours.resize(typeCount, WHITE);
    Color defaultColours[9] = { RED, BLUE, YELLOW, PURPLE, GREEN, ORANGE, PINK, RAYWHITE, LIGHTGRAY };
    for (int i = 0; i < typeCount; i++)
        colours[i] = defaultColours[i];

    // allocate vectors and randomise attractions and positions
    attractions.resize(typeCount, std::vector<float>(typeCount, 0.0f));
    types.resize(count, 0);
    velocities.resize(count, { 0.0f, 0.0f });
    positions.resize(count, { 0.0f, 0.0f });
    randomise();

    // initialise particle texture
    Image temp = GenImageColor(64, 64, BLANK);
    ImageDrawCircle(&temp, 32, 32, 32, WHITE);
    particleTexture = LoadTextureFromImage(temp);
    UnloadImage(temp);
}

void ParticleLife::update()
{
    // ========================================= PARTICLE INTERACTION
    // for each particle
    for (int i = 0; i < count; i++) {

        // cache particle variables
        const int type = types[i];
        const float xPos = positions[i].x;
        const float yPos = positions[i].y;
        float xVelInc = 0.0f;
        float yVelInc = 0.0f;
        std::vector<float>& attractionArray = attractions[type];

        // for each other particle
        for (int j = 0; j < count; j++) {
            if (i == j) continue;

            // get distance from other particle
            float xDist = positions[j].x - xPos;
            float yDist = positions[j].y - yPos;
            float sqDist = xDist*xDist + yDist*yDist;

            // if other particle within acting range
            if (sqDist <= 4.0f) {
                float distance = sqrtf(sqDist);

                // repulse if in inner radius, otherwise apply attraction
                float coef = (distance <= innerRadius)
                    ? 1.0f - innerRadius / distance
                    : attractionArray[types[j]] * (distance - innerRadius);

                // increment normalised force to particle
                xVelInc += coef * (xDist / distance);
                yVelInc += coef * (yDist / distance);
            }
        }

        // apply accumulative force to original particle
        velocities[i].x += xVelInc;
        velocities[i].y += yVelInc;

    }
    
    // ===================================== OTHER FORCES AND BOUDNS
    // for each particle
    const float invResistance = 1.0f - resistance;
    for (int i = 0; i < count; i++) {

        // cache variables
        float xVel = velocities[i].x;
        float yVel = velocities[i].y;
        float xPos = positions[i].x;
        float yPos = positions[i].y;

        // apply resistance
        xVel *= invResistance;
        yVel *= invResistance;

        // loop boundaries
        if (xPos < 0.0f) xPos = bounds;
        if (xPos > bounds) xPos = 0.0f;
        if (yPos < 0.0f) yPos = bounds;
        if (yPos > bounds) yPos = 0.0f;

        // apply new position and velocity
        xPos += step * xVel;
        yPos += step * yVel;
        positions[i] = { xPos, yPos };
        velocities[i] = { xVel, yVel };
        
    }
}

void ParticleLife::draw()
{
    rlSetTexture(particleTexture.id);
    rlBegin(RL_QUADS);

        for (int i = 0; i < count; i++) {
            Vector2 pos = positions[i];
            Color colour = colours[types[i]];

            // set colour of particle and set normal towards viewer
            rlColor4ub(colour.r, colour.g, colour.b, 255);
            rlNormal3f(0.0f, 0.0f, 1.0f);

            rlTexCoord2f(0.0f, 0.0f);               // top left
            rlVertex2f(pos.x-0.05f, pos.y-0.05f);

            rlTexCoord2f(0, 1.0f);                  // bottom left
            rlVertex2f(pos.x-0.05f, pos.y+0.05f);

            rlTexCoord2f(1.0f, 1.0f);               // bottom right
            rlVertex2f(pos.x+0.05f, pos.y+0.05f);

            rlTexCoord2f(1.0f, 0.0f);               // top right
            rlVertex2f(pos.x+0.05f, pos.y-0.05f);

        }

    rlSetTexture(0);
    rlEnd();
}

void ParticleLife::randomise()
{
    // randomise attraction values
    for (int i = 0; i < typeCount; i ++)
        for (int j = 0; j < typeCount; j ++)
            attractions[i][j] = GetRandomValue(-250, 250) / 100.0f;

    // randomise positions and reset velocity
    for (int i = 0; i < count; i++) {
        types[i] = (i % typeCount);
        velocities[i] = { 0.0f, 0.0f };
        positions[i] = { GetRandomValue(0, 100*bounds) / 100.0f,
                         GetRandomValue(0, 100*bounds) / 100.0f };
    }
}