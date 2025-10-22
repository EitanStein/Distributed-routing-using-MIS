#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <functional>
#include <stop_token>
#include <vector>
#include <atomic>

class ThreadPool
{
private:
    std::mutex queue_lock;
    std::vector<std::jthread> threads;
    std::condition_variable queue_cv;
    std::queue<std::function<void()>> task_queue;

    std::atomic<size_t> num_active_tasks;
    std::condition_variable tasks_done_cv;

    void ThreadLoop(std::stop_token stoken);
public:
    ThreadPool(size_t thread_pool_size);
    ~ThreadPool();
    bool IsTaskQueueEmpty();
    void WaitForEmptyQueue();
    void AddTask(std::function<void()> task);
};