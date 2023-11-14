#pragma once

#include <raylib.h>
#include <rlgl.h>

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>


class ParticleLife
{

 public:

    struct Settings {
        int typeCount;
        int count;
        float innerRadius;
        float resistance;
        float step;
        float gridSize;
        std::vector<std::vector<float>> attractions;
    };

    void init(Settings settings);
    void update();
    void draw();

    int getTypeCount() const;
    int getCount() const;
    float getResistance() const;
    float getInnerRadius() const;
    float getStep() const;
    int getGridSize() const;

    void randomisePositions();
    void randomiseAttractions();
    void randomiseAll();

    void printCell(int r, int c);

 private:

    int typeCount;
    int count;
    float resistance;
    float innerRadius;
    float step;
    float bounds;
    int gridSize;
    
    std::vector<Color> colours;
    std::vector<std::vector<float>> attractions;

    std::vector<int> types;
    std::vector<Vector2> positions;
    std::vector<Vector2> velocities;
    
    Texture2D particleTexture;

    int** gridCounts;
    std::vector<int>** gridIds;

    class ThreadPool
    {
     public:

        ~ThreadPool();

        void init(int particleCount);
        
        template<class F, class... Args> void addTask(F&& f, Args&&... args);


    
     private:

        void workerFunction();

        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queueMutex;
        std::condition_variable condition;
        bool stop;

        int threadCount;
        int particlesPerThread;
        int particlesPerFinalThread;
    };

    ThreadPool threadPool;

    void initGrid();
    void initColours();
    void initTexture();

    void sectionInteraction(int start, int end);
    void sectionApplyForce(int start, int end);

    inline int gridHash(float coord);

    void mapGrid();

};