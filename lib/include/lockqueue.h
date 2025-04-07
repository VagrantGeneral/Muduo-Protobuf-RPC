#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

//异步写日志的日志队列
template<typename T>
class LockQueue {
public:
    //进队列
    void Push(const T& data) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(data);
        m_condvariable.notify_one();
    }

    //出队列
    T Pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty()) {
            //队列为空，进入wait状态
            m_condvariable.wait(lock);
        }

        T data = m_queue.front();
        m_queue.pop();
        return data;
    }

private:
    std::queue<T> m_queue;                  //队列
    std::mutex m_mutex;                     //互斥锁
    std::condition_variable m_condvariable; //条件变量
};