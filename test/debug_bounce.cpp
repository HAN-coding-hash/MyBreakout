#include <cstdio>
#include "../src/Ball.h"

int main() {
    printf("测试1: 向左移动撞左墙\n");
    // 创建一个球，不调用 Launch，手动设置位置和速度
    // 但 Ball 类没有 SetPosition 方法，需要添加
    // 临时方案：修改 Ball.h 添加 SetPosition 方法
    
    // 由于我们无法直接设置位置，这个测试需要修改 Ball 类
    
    printf("注意：需要为 Ball 类添加 SetPosition 方法才能完整测试\n");
    
    printf("\n替代测试：检查边界反弹逻辑\n");
    
    // 模拟边界碰撞
    float x = 6;
    float radius = 5;
    float speed_x = -5;
    
    printf("  左边界条件: x - radius = %.1f <= 0 ? %s\n", 
           x - radius, (x - radius <= 0) ? "是" : "否");
    
    if (x - radius <= 0) {
        x = radius;
        speed_x = -speed_x;  // 或 abs(speed_x)
        printf("  触发反弹，新速度: %.1f, 新位置: %.1f\n", speed_x, x);
    } else {
        printf("  未触发反弹\n");
    }
    
    return 0;
}
