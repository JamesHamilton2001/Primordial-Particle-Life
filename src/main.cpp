#include "App.hpp"


void initSettings();

ParticleLife::Settings redDeathSmall;


int main()
{
    initSettings();

    while (true)
    {
        int width = 800;
        int height = 800;
        int fpsTarget = 0;
        
        ParticleLife::Settings settings;

        int option;
        std::cin >> option;

        switch(option)
        {
            case 1:
                settings = redDeathSmall;
                break;
            default:
                continue;
                break;
        }

        InitWindow(width, height, "Particle Life");
        SetTargetFPS(fpsTarget);

        App app(width, height, fpsTarget);

        ParticleLife particleLife(settings);

        while (!WindowShouldClose())
        {
            app.update(particleLife);
            app.draw(particleLife);
        }

        CloseWindow();
        
    }

    return 0;

}

void initSettings()
{
    redDeathSmall.types = 5;
    redDeathSmall.size = 12;
    redDeathSmall.count = 512;
    redDeathSmall.resistance = 0.00125f;
    redDeathSmall.innerRadius = 0.3f;
    redDeathSmall.step = 0.0005f;

    redDeathSmall.attractions.resize(redDeathSmall.types, std::vector<float>(redDeathSmall.types, 0.0f));

    redDeathSmall.attractions[0][0] = 0.04f;
    for (int i = 1; i < redDeathSmall.types; i++)
        redDeathSmall.attractions[0][i] = 0.03f;
    for (int i = 1; i < redDeathSmall.types; i++)
        redDeathSmall.attractions[i][0] = -0.02f;
    for (int i = 1; i < redDeathSmall.types; i++)
        for (int j = 0; j < redDeathSmall.types; j++)
            redDeathSmall.attractions[i][j] = -0.01f + (i+j) % 4 * 0.005f;

    redDeathSmall.seed = 1234;
}
