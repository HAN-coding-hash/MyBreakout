#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <atomic>

std::queue<int> g_queue;
std::mutex g_mutex;
std::condition_variable g_cv;
std::atomic<bool> g_done(false);

void producer() {
    for (int i = 1; i <= 10; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            g_queue.push(i);
            std::cout << "生产: " << i << std::endl;
        }
        g_cv.notify_one();
    }
    g_done = true;
    g_cv.notify_all();
}

void consumer(int id) {
    while (!g_done || !g_queue.empty()) {
        std::unique_lock<std::mutex> lock(g_mutex);
        g_cv.wait(lock, []{ return !g_queue.empty() || g_done; });
        
        while (!g_queue.empty()) {
            int value = g_queue.front();
            g_queue.pop();
            lock.unlock();
            std::cout << "消费者 " << id << " 消费: " << value << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
            lock.lock();
        }
    }
}

int main() {
    std::cout << "=== 生产者-消费者模式 ===" << std::endl;
    
    std::thread prod(producer);
    std::thread cons1(consumer, 1);
    std::thread cons2(consumer, 2);
    
    prod.join();
    cons1.join();
    cons2.join();
    
    std::cout << "所有任务完成" << std::endl;
    return 0;
}
