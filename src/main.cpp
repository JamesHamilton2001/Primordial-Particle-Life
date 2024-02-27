#include "App.hpp"


void initSettings();

ParticleLife::Settings dbg;
ParticleLife::Settings bigBoy;
ParticleLife::Settings soup;


int main()
{
    initSettings();

    while (true)
    {
        int width = 1024;
        int height = 1024;
        int fpsTarget = 0;
        
        ParticleLife::Settings settings;

        int option;
        option = 2;
        // std::cin >> option;

        switch(option)
        {
            case 1:     settings = dbg;         break;
            case 2:     settings = bigBoy;      break;
            case 3:     settings = soup;        break;
            default:    continue;               break;
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
    dbg.types = 3;
    dbg.size = 4;
    dbg.count = 15;
    dbg.resistance = 0.001f;
    dbg.innerRadius = 0.5f;
    dbg.step = 0.001f;
    dbg.attractions = {
        {  0.005,  0.002,  -0.002 },
        { -0.002,  0.005,   0.002 },
        {  0.002, -0.002,   0.005 }
    }; dbg.seed = 1234;

    bigBoy.types = 5;
    bigBoy.size = 24;
    bigBoy.count = 3072;
    bigBoy.resistance = 0.001f;
    bigBoy.innerRadius = 0.55f;
    bigBoy.step = 0.00002f;
    bigBoy.attractions = {
        {  0.02,  0.15,  0.15,  0.20, -0.10 }, 
        { -0.03,  0.07,  0.10, -0.05,  0.06 },
        {  0.05,  0.10, -0.02, -0.05,  0.05 },
        {  0.10, -0.05,  0.04,  0.05,  0.10 },
        { -0.05, -0.10,  0.05,  0.10, -0.05 }
    }; bigBoy.seed = 1234;

    soup.types = 5;
    soup.size = 8;
    soup.count = 2048;
    soup.resistance = 0.00125f;
    soup.innerRadius = 0.5f;
    soup.step = 0.002f;
    soup.attractions = {
        {  0.0100,  0.0014, -0.0014,  0.0024, -0.0024  },
        { -0.0024,  0.0100,  0.0013, -0.0013,  0.0023  },
        {  0.0024, -0.0023,  0.0100,  0.0012, -0.0012  },
        { -0.0014,  0.0023, -0.0022,  0.0100,  0.0011  },
        {  0.0014, -0.0013,  0.0022, -0.0021,  0.0100  }
    }; soup.seed = 1234;
}