#include "App.hpp"


void redDeathSmall(int& types, int& size, int& count, float& resistance, float& innerRadius, float& step, std::vector<std::vector<float>>& attractions, int& seed);


int main()
{
    int width = 800;
    int height = 800;
    int fpsTarget = 0;

    int option;

    int types;
    int size;
    int count;
    float resistance;
    float innerRadius;
    float step;
    std::vector<std::vector<float>> attractions;
    int seed;

    while (true)
    {
        std::cin >> option;

        switch(option)
        {
            case 1:
                redDeathSmall(types, size, count, resistance, innerRadius, step, attractions, seed);
                break;
            default:
                continue;
                break;
        }

        InitWindow(width, height, "Particle Life");
        SetTargetFPS(fpsTarget);

        App app(width, height, fpsTarget);

        ParticleLife particleLife(
            types,                             
            size,                             
            count,                            
            resistance,                       
            innerRadius,                      
            step,                             
            attractions,                      
            seed                              
        );

        while (!WindowShouldClose())
        {
            app.update(particleLife);
            app.draw(particleLife);
        }

        CloseWindow();
        
    }

    return 0;

}

void redDeathSmall(int& types, int& size, int& count, float& resistance, float& innerRadius, float& step, std::vector<std::vector<float>>& attractions, int& seed)
{
    types       = 5;
    size        = 16;
    count       = 512;
    resistance  = 0.0025f;
    innerRadius = 0.5f;
    step        = 0.0025f;

    attractions.resize(types, std::vector<float>(types, 0.0f));

    attractions[0][0] = 0.06f;
    for (int i = 1; i < types; i++)
        attractions[0][i] = 0.04f;
    for (int i = 1; i < types; i++)
        attractions[i][0] = -0.04f;
    for (int i = 1; i < types; i++)
        for (int j = 0; j < types; j++)
            attractions[i][j] = -0.01f + (i+j) % 4 * 0.01f;

    seed = 1234;
    
}