#include <iostream>
#include <thread>
#include <chrono>

// 线程函数
void worker(int id) {
    for (int i = 0; i < 3; i++) {
        std::cout << "线程 " << id << " 工作中... 第" << i+1 << "次" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    std::cout << "=== 线程基础演示 ===" << std::endl;
    
    // 创建两个线程
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    
    // 等待线程完成
    t1.join();
    t2.join();
    
    std::cout << "所有线程完成" << std::endl;
    return 0;
}
