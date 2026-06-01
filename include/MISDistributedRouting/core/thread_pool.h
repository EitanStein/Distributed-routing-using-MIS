#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <stop_token>
#include <vector>
#include <atomic>
#include "types.h"


template <typename T>
class TaskQueue{
private:
    size_t front_index;
    size_t back_index;
    size_t queue_size;
    std::vector<T> main_queue;
    std::queue<T> backup_queue;

    void increment_front_index(){
        ++front_index;
        if(front_index == main_queue.size())
            front_index = 0;
    }
    void increment_back_index(){
        ++back_index;
        if(back_index == main_queue.size())
            back_index = 0;
    }

public:
    TaskQueue() : front_index(0), back_index(0), queue_size(0) {}
    void allocate_queue(size_t size, T default_value) {
        main_queue = std::vector<T>(size, default_value);
    }
    [[nodiscard]] bool empty() const {
        return queue_size == 0;
    }
    template<typename... Args>
    void emplace(Args&&... args){
        if(queue_size < main_queue.size()){
            main_queue[back_index] = T(std::forward<Args>(args)...);
            increment_back_index();
        }
        else
            backup_queue.emplace(std::forward<Args>(args)...);

        ++queue_size;
    }
    [[nodiscard]] T& front(){
        if(queue_size == 0) [[unlikely]]
            throw std::runtime_error("empty task queue");
        return main_queue[front_index];
    }
    [[nodiscard]] T const& front() const{
        if(queue_size == 0) [[unlikely]]
            throw std::runtime_error("empty task queue");
        return main_queue.at(front_index);
    }
    void pop(){
        if(queue_size == 0) [[unlikely]] // TODO remove?
            throw std::runtime_error("empty task queue");
        increment_front_index();
        
        if(!backup_queue.empty()){
            main_queue[back_index] = std::move(backup_queue.front());
            backup_queue.pop();
            increment_back_index();
        }

        --queue_size;
    }
};


class ThreadPool
{
private:
    size_t thread_pool_size;
    std::mutex queue_lock;
    std::vector<std::jthread> threads;
    std::condition_variable_any queue_cv;
    TaskQueue<MessagerNode*> task_queue;

    std::atomic<size_t> num_active_tasks; // TODO needs to be atomic?
    std::condition_variable tasks_done_cv;

    void ThreadLoop(std::stop_token stoken);
public:
    ThreadPool(size_t thread_pool_size);
    ~ThreadPool();

    void InitPool(size_t task_reserve_size);
    void StopAllThreads();

    bool IsTaskQueueEmpty();
    void WaitForEmptyQueue();
    
    void AddTask(MessagerNode*);
};