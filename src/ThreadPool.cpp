#include "ThreapPool.hpp"

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>



void ThreadPool::init(int particleCount)
{
    stop = false;
    threadCount = std::thread::hardware_concurrency();
    particlesPerThread = particleCount / threadCount;
    particlesPerFinalThread = particleCount - (threadCount-1) * particlesPerThread;
    for (size_t i = 0; i < threadCount; ++i)
        workers.emplace_back(std::bind(&ThreadPool::workerFunction, this));
}




template<class Func, class ...Args>
void ThreadPool::addTask(Func&& func, Args&& ...args)
{
    auto task = std::make_shared<std::packaged_task<void()>>(
                    std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
                );
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace([task] { (*task)(); });
    }
    condition.notify_one();
}




void ThreadPool::waitAll()
{
    std::unique_lock<std::mutex> lock(queueMutex);
    condition.wait(lock, [this]() { return tasks.empty(); });
}




void ThreadPool::workerFunction()
{
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() { return stop || !tasks.empty(); });

            if (stop && tasks.empty()) return;

            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}




ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers)
        worker.join();
}

