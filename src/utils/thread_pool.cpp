#include "MISDistributedRouting/utils/thread_pool.h"

#include <ranges>


ThreadPool::ThreadPool(size_t thread_pool_size) : num_active_tasks(0)
{
    threads.reserve(thread_pool_size);
    for(auto i : std::views::iota(size_t{0}, thread_pool_size))
    {
        threads.emplace_back([this](std::stop_token stoken){ 
            ThreadLoop(stoken); 
        });
    }
}



ThreadPool::~ThreadPool()
{
    for(auto& th : threads)
        th.request_stop();
    
    queue_cv.notify_all();
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
    queue_cv.notify_one();
}

void ThreadPool::WaitForEmptyQueue()
{
    std::unique_lock<std::mutex> lock(queue_lock);
    tasks_done_cv.wait(lock, [this](){
        return this->task_queue.empty() && num_active_tasks == 0;
    });
}

void ThreadPool::ThreadLoop(std::stop_token stoken)
{
    while (!stoken.stop_requested())
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_lock);
            queue_cv.wait(lock, [this, &stoken](){
                return stoken.stop_requested() || !this->task_queue.empty();
            });
            
            if (stoken.stop_requested())
                return;

            ++num_active_tasks;
            task = std::move(task_queue.front());
            task_queue.pop();
            
        }
        task();

        {
            --num_active_tasks;
            std::unique_lock<std::mutex> lock(queue_lock);
            if(task_queue.empty() && num_active_tasks == 0)
                tasks_done_cv.notify_all();
        }

    }
}