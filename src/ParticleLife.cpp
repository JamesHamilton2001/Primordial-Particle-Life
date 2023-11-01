#include "ParticleLife.hpp"

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <vector>
#include <iostream>


void ParticleLife::init(int particleTypes, int particleCount, int gridSize)
{
    // simulation settings
    step = 0.00005f;
    resistance = 0.001f;
    count = particleCount;
    bounds = 2.0f * gridSize;

    // randomise attraction values
    attractions.resize(particleTypes, std::vector<float>(particleTypes, 0.0f));
    for (int i = 0; i < particleTypes; i ++)
        for (int j = 0; j < particleTypes; j ++)
            attractions[i][j] = 0.05f + GetRandomValue(0, 10) / 100.0f;
    
    // assign colours
    colours.resize(particleTypes, WHITE);
    Color defaultColours[9] = { RED, BLUE, YELLOW, PURPLE, GREEN, ORANGE, PINK, RAYWHITE, LIGHTGRAY };
    for (int i = 0; i < particleTypes; i++)
        colours[i] = defaultColours[i];

    // initalise random particle values
    for (int i = 0; i < count; i++) {
        velocities.push_back({ 0, 0 });
        types.push_back(i % particleTypes);
        positions.push_back({ GetRandomValue(0, 100*bounds) / 100.0f,
                              GetRandomValue(0, 100*bounds) / 100.0f });
    }

    // initialise particle texture
    Image temp = GenImageColor(64, 64, BLANK);
    ImageDrawCircle(&temp, 32, 32, 32, WHITE);
    particleTexture = LoadTextureFromImage(temp);
    UnloadImage(temp);
}

void ParticleLife::update()
{
    // for each particle
    for (int i = 0; i < count; i++) {

        // cache particle variables
        const int type = types[i];
        const float xPos = positions[i].x;
        const float yPos = positions[i].y;
        float xVelInc = 0.0f;
        float yVelInc = 0.0f;

        // for each other particle
        for (int j = 0; j < count; j++) {
            if (i == j) continue;

            // get distance from other particle
            float xDist = positions[j].x - xPos;
            float yDist = positions[j].y - yPos;
            float sqDist = xDist*xDist + yDist*yDist;
            std::vector<float>& attractionArray = attractions[type];

            // if other particle within acting range
            if (sqDist <= 4.0f) {
                float distance = sqrtf(sqDist);

                // repulse if in inner radius, otherwise attract
                float reactionCoef = (distance <= 0.5f)
                    ? 1.0f - 0.5f / distance
                    :  attractionArray[types[j]] * (distance - 0.5f);
                
                // apply normalised force to other particle
                float xForce = reactionCoef * xDist / distance;
                float yForce = reactionCoef * yDist / distance;
                xVelInc += xForce;
                yVelInc += yForce;
                velocities[j].x -= xForce;
                velocities[j].y -= yForce;
            }
        }

        // apply accumulative force to original particle
        velocities[i].x += xVelInc;
        velocities[i].y += yVelInc;
    }

    // for each particle
    const float invResistance = 1.0f - resistance;
    for (int i = 0; i < count; i++) {

        // cache variables
        float xVelocity = velocities[i].x;
        float yVelocity = velocities[i].y;

        // apply friction
        xVelocity *= invResistance;
        yVelocity *= invResistance;

        // apply movement
        positions[i].x += step * xVelocity;
        positions[i].y += step * yVelocity;

        // bounce if bounds reached
        if (positions[i].x < 0 || positions[i].x > bounds)
            xVelocity *= -1.0f;
        if (positions[i].y < 0 || positions[i].y > bounds)
            yVelocity *= -1.0f;

        // apply calculated velocity
        velocities[i].x = xVelocity;
        velocities[i].y = yVelocity;
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

            rlTexCoord2f(0.0f, 0.0f);       // top left
            rlVertex2f(pos.x-0.05f, pos.y-0.05f);

            rlTexCoord2f(0, 1.0f);          // bottom left
            rlVertex2f(pos.x-0.05f, pos.y+0.05f);

            rlTexCoord2f(1.0f, 1.0f);       // bottom right
            rlVertex2f(pos.x+0.05f, pos.y+0.05f);

            rlTexCoord2f(1.0f, 0.0f);       // top right
            rlVertex2f(pos.x+0.05f, pos.y-0.05f);

        }

    rlSetTexture(0);
    rlEnd();
}