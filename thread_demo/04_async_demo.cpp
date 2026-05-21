#include <iostream>
#include <future>
#include <thread>
#include <chrono>

// 模拟耗时计算
int long_computation(int input) {
    std::cout << "开始计算..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "计算完成" << std::endl;
    return input * input;
}

int main() {
    std::cout << "=== std::async 演示 ===" << std::endl;
    
    // 异步执行任务
    std::future<int> result = std::async(std::launch::async, long_computation, 10);
    
    std::cout << "主线程继续做其他事情..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "主线程其他工作完成" << std::endl;
    
    // 等待异步结果
    int value = result.get();
    std::cout << "计算结果: " << value << std::endl;
    
    return 0;
}
