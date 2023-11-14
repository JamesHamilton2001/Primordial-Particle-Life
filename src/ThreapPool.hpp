#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>




class ThreadPool
{
    public:

    ~ThreadPool();

    void init(int particleCount);
    
    template<class F, class... Args> void addTask(F&& f, Args&&... args);
    void waitAll();

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
