#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <functional>
#include <stop_token>
#include <vector>

class ThreadPool
{
private:
    std::mutex queue_lock;
    std::vector<std::jthread> threads;
    std::condition_variable cv;
    std::queue<std::function<void()>> task_queue;

    void ThreadLoop(std::stop_token stoken);
public:
    ThreadPool(size_t thread_pool_size);
    ~ThreadPool();
    bool IsTaskQueueEmpty();
    void AddTask(std::function<void()> task);
};