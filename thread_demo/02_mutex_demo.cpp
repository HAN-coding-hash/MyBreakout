#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

std::mutex g_mutex;
int g_counter = 0;

void unsafe_increment(int id) {
    for (int i = 0; i < 100000; i++) {
        // 没有加锁，会有数据竞争
        g_counter++;
    }
    std::cout << "线程 " << id << " 完成（无锁）" << std::endl;
}

void safe_increment(int id) {
    for (int i = 0; i < 100000; i++) {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_counter++;
    }
    std::cout << "线程 " << id << " 完成（有锁）" << std::endl;
}

int main() {
    // 无锁版本（会有数据竞争）
    std::cout << "=== 无锁版本（可能结果错误）===" << std::endl;
    g_counter = 0;
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; i++) {
        threads.emplace_back(unsafe_increment, i);
    }
    for (auto& t : threads) t.join();
    std::cout << "最终计数（预期400000）: " << g_counter << std::endl;
    
    // 有锁版本（正确）
    std::cout << "\n=== 有锁版本（结果正确）===" << std::endl;
    g_counter = 0;
    threads.clear();
    for (int i = 0; i < 4; i++) {
        threads.emplace_back(safe_increment, i);
    }
    for (auto& t : threads) t.join();
    std::cout << "最终计数（预期400000）: " << g_counter << std::endl;
    
    return 0;
}
