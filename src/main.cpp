#include "App.hpp"


void initSettings();

ParticleLife::Settings redDeathSmall;
ParticleLife::Settings soup;


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
        // std::cin >> option;
        option = 1;

        switch(option)
        {
            case 1:
                settings = redDeathSmall;
                break;
            case 2:
                settings = soup;
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
    redDeathSmall.size = 16;
    redDeathSmall.count = 1024;
    redDeathSmall.resistance = 0.0015f;
    redDeathSmall.innerRadius = 0.6f;
    redDeathSmall.step = 0.0003f;

    redDeathSmall.attractions.resize(redDeathSmall.types, std::vector<float>(redDeathSmall.types, 0.0f));

    for (int i = 1; i < redDeathSmall.types; i++)
        redDeathSmall.attractions[0][i] = 0.5f;
    for (int i = 1; i < redDeathSmall.types; i++)
        redDeathSmall.attractions[i][0] = -0.3f;
    for (int i = 1; i < redDeathSmall.types; i++)
        for (int j = 0; j < redDeathSmall.types; j++)
            redDeathSmall.attractions[i][j] = -0.05f + (i+j)%4 * 0.05f;

    redDeathSmall.seed = 1234;

    soup.types = 5;
    soup.size = 6;
    soup.count = 1024;
    soup.resistance = 0.00125f;
    soup.innerRadius = 0.5f;
    soup.step = 0.002f;

    // soup.attractions.resize(soup.types, std::vector<float>(soup.types, 0.0f));

    soup.attractions =
    {
        {  0.0100,  0.0014, -0.0014,  0.0024, -0.0024  },
        { -0.0024,  0.0100,  0.0013, -0.0013,  0.0023  },
        {  0.0024, -0.0023,  0.0100,  0.0012, -0.0012  },
        { -0.0014,  0.0023, -0.0022,  0.0100,  0.0011  },
        {  0.0014, -0.0013,  0.0022, -0.0021,  0.0100  }
    };

    soup.seed = 1234;

}