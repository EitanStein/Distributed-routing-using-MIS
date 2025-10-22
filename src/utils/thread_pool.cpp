#include "MISDistributedRouting/utils/thread_pool.h"

#include <ranges>


ThreadPool::ThreadPool(size_t thread_pool_size)
{
    threads.reserve(thread_pool_size);
    for(auto i : std::views::iota(size_t{0}, thread_pool_size))
    {
        threads.emplace_back(&ThreadPool::ThreadLoop, this);
    }
}



ThreadPool::~ThreadPool()
{
    for(auto& th : threads)
    {
        th.request_stop();
    }
    cv.notify_all();
}



bool ThreadPool::IsTaskQueueEmpty()
{
    std::unique_lock<std::mutex> lock(queue_lock);
    return task_queue.empty();
}



void ThreadPool::AddTask(std::function<void()> task)
{
    std::unique_lock<std::mutex> lock(queue_lock);
    task_queue.push(std::move(task));
    cv.notify_one();
}


void ThreadPool::ThreadLoop(std::stop_token stoken)
{
    while (!stoken.stop_requested())
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_lock);
            cv.wait(lock, [this, &stoken](){
                return stoken.stop_requested() || !this->task_queue.empty();
            });
            
            if (stoken.stop_requested())
                return;

            task = std::move(task_queue.front());
            task_queue.pop();
            
        }
        task();
    }
}