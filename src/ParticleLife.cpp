#include "ParticleLife.hpp"

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <vector>
#include <iostream>



void ParticleLife::init(Settings settings)
{
    this->typeCount   = settings.typeCount;
    this->count       = settings.count;
    this->innerRadius = settings.innerRadius;
    this->resistance  = settings.resistance;
    this->step        = settings.step;
    this->bounds      = 2.0f * settings.gridSize;
    this->gridSize    = settings.gridSize;
    this->attractions = settings.attractions;

    types.resize(count, 0);
    velocities.resize(count, { 0.0f, 0.0f });
    positions.resize(count, { 0.0f, 0.0f });
    randomisePositions();
    
    initColours();
    initTexture();
    initGrid();

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

        // get particle row col in grid
        int row = gridHash(positions[i].y);
        int col = gridHash(positions[i].x);

        // iterate over neighboring row and collumn
        for (int j = -1; j <= 1; j++) {
            int r = (row + j + gridSize) % gridSize;
            for (int k = -1; k <= 1; k++) {
                int c = (col + k + gridSize) % gridSize;

                // get ids from neighbor cell
                int cellCount = gridCounts[r][c];
                for (int l = 0; l < cellCount; l++) {
                    int id = gridIds[r][c][l];

                    if (j == 0 && k == 0 && i == id) continue;

                    // get distance from other particle
                    float xDist = positions[id].x - xPos;
                    float yDist = positions[id].y - yPos;
                    float sqDist = xDist*xDist + yDist*yDist;

                    // if  witin acting range
                    if (sqDist <= 4.0f) {
                        float distance = sqrtf(sqDist);

                        // repulse if within inner radius, otherwise apply attraction
                        float coef = (distance <= innerRadius)
                            ? 1.0f - innerRadius / distance
                            : attractionArray[types[id]] * (distance - innerRadius);

                        // increment normalised force
                        xVelInc += coef * (xDist / distance);
                        yVelInc += coef * (yDist / distance);
                    }
                }
            }
        }

        // apply accumulated force to original particle
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

        // bounce on boundaries
        if (xPos < 0.0f)   xPos = 0.0f,   xVel *= -1.0f;
        if (xPos > bounds) xPos = bounds, xVel *= -1.0f;
        if (yPos < 0.0f)   yPos = 0.0f,   yVel *= -1.0f;
        if (yPos > bounds) yPos = bounds, yVel *= -1.0f;
 
        // apply new position and velocity
        xPos += step * xVel;
        yPos += step * yVel;
        positions[i] = { xPos, yPos };
        velocities[i] = { xVel, yVel };
        
    }

    mapGrid();

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

            // assign tex coords to vertexes
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



int ParticleLife::getTypeCount() const { return typeCount; }

int ParticleLife::getCount() const { return count; }

float ParticleLife::getResistance() const { return resistance; }

float ParticleLife::getInnerRadius() const { return innerRadius; }

float ParticleLife::getStep() const { return step; }

int ParticleLife::getGridSize() const { return bounds/2.0f; }



void ParticleLife::randomisePositions()
{
    for (int i = 0; i < count; i++) 
        types[i] = (i % typeCount),
        velocities[i] = { 0.0f, 0.0f },
        positions[i] = { GetRandomValue(0, 100*bounds) / 100.0f,
                         GetRandomValue(0, 100*bounds) / 100.0f };
    std::cout << "Randomise Positions" << std::endl << std::endl;
}

void ParticleLife::randomiseAttractions()
{
    for (int i = 0; i < typeCount; i ++)
        for (int j = 0; j < typeCount; j ++)
            attractions[i][j] = GetRandomValue(-10, 10) / 10.0f;

    std::cout << "Randomise Attractions" << std::endl;
    for (std::vector<float>& set : attractions) {
        for (float val : set)
            std::cout << val << ", ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void ParticleLife::randomiseAll()
{
    randomisePositions();
    randomiseAttractions();
}

void ParticleLife::printCell(int r, int c)
{
    std::cout << "Cell[" << r << "][" << c << "]" << std::endl;
    for (int i = 0; i < gridCounts[r][c]; i++)
        std::cout << types[gridIds[r][c][i]] << ", " << positions[i].x << ", " << positions[i].y << std::endl;
    std::cout << std::endl;
}



void ParticleLife::initGrid()
{
    gridCounts = new int*[gridSize];
    gridIds = new std::vector<int>*[gridSize];

    for (int i = 0; i < gridSize; i++) {
        gridCounts[i] = new int[gridSize];
        gridIds[i] = new std::vector<int>[gridSize];

        for (int j = 0; j < gridSize; j++) {
            gridCounts[i][j] = 0;
            gridIds[i][j].resize((3 * count/gridSize)/2, -1);
        }
    }
}

void ParticleLife::initColours()
{
    Color defaultColours[9] = { RED, BLUE, YELLOW, PURPLE, GREEN, ORANGE, PINK, RAYWHITE, LIGHTGRAY };
    colours.resize(typeCount, WHITE);
    for (int i = 0; i < typeCount; i++)
        colours[i] = defaultColours[i];
}

void ParticleLife::initTexture()
{
    Image temp = GenImageColor(64, 64, BLANK);
    ImageDrawCircle(&temp, 32, 32, 32, WHITE);
    particleTexture = LoadTextureFromImage(temp);
    UnloadImage(temp);
}

inline int ParticleLife::gridHash(float coord)
{
    return abs((int) (coord/2.0f) % gridSize);
}

// !! idk why but it only works if increment in the else statement
void ParticleLife::mapGrid()
{
    // reset counts
    for (int r = 0; r < gridSize; r++)
        for (int c = 0; c < gridSize; c++)
            gridCounts[r][c] = 0;
    
    // remap ids and recaculate counts
    for (int i = 0; i < count; i++) {
        int r = gridHash(positions[i].y);
        int c = gridHash(positions[i].x);

        // if capacity reached, resize and add id
        if (gridCounts[r][c] >= gridIds[r][c].capacity())
            gridIds[r][c].push_back(i);
        
        // otherwise just map id
        else gridIds[r][c][gridCounts[r][c]++] = i;
    }
}
