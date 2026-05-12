#include "MISDistributedRouting/utils/thread_pool.h"

#include "MISDistributedRouting/core/node.h"
#include <ranges>


ThreadPool::ThreadPool(size_t thread_pool_size) : thread_pool_size(thread_pool_size) ,num_active_tasks(0)
{
    InitPool();
}


void  ThreadPool::InitPool()
{
    threads.clear();
    threads.reserve(thread_pool_size);
    for([[maybe_unused]] auto i : std::views::iota(size_t{0}, thread_pool_size))
    {
        threads.emplace_back([this](std::stop_token stoken){ 
            ThreadLoop(stoken); 
        });
    }
}

void ThreadPool::StopAllThreads()
{
    for(auto& th : threads)
        th.request_stop();
    
    queue_cv.notify_all();
}



ThreadPool::~ThreadPool()
{
    StopAllThreads();
}



bool ThreadPool::IsTaskQueueEmpty()
{
    std::unique_lock<std::mutex> lock(queue_lock);
    return task_queue.empty();
}



void ThreadPool::AddTask(MessagerNode* node_ptr, MessagerNodeTask::Task task)
{
    std::unique_lock<std::mutex> lock(queue_lock);
    task_queue.emplace(node_ptr, task);
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
        MessagerNode* node_ptr = nullptr;
        MessagerNodeTask::Task task;
        {
            std::unique_lock<std::mutex> lock(queue_lock);
            queue_cv.wait(lock, stoken, [this](){
                return !this->task_queue.empty();
            });
            
            if (stoken.stop_requested())
                return;

            ++num_active_tasks;
            node_ptr = task_queue.front().first;
            task = task_queue.front().second;
            task_queue.pop();
            
        }
        node_ptr->PerformTask(task);

        {
            std::unique_lock<std::mutex> lock(queue_lock);
            --num_active_tasks;
            if(task_queue.empty() && num_active_tasks == 0)
                tasks_done_cv.notify_all();
        }

    }
}