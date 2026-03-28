#include <cstdio>
#include <cmath>

// 修复后的边界反弹逻辑
void bounceEdge(float &x, float &speedX, float radius, int screenWidth) {
    printf("    检查: x=%.1f, radius=%.1f, x-radius=%.1f, x+radius=%.1f\n", 
           x, radius, x - radius, x + radius);
    
    // 左边界
    if (x - radius <= 0) {
        x = radius;
        speedX = std::abs(speedX);
        printf("    ✅ 触发左边界: 新速度=%.1f, 新位置=%.1f\n", speedX, x);
    }
    // 右边界
    if (x + radius >= screenWidth) {
        x = screenWidth - radius;
        speedX = -std::abs(speedX);
        printf("    ✅ 触发右边界: 新速度=%.1f, 新位置=%.1f\n", speedX, x);
    }
}

int main() {
    printf("\n========== 边界反弹逻辑测试 ==========\n\n");
    
    int passed = 0;
    int failed = 0;
    float radius = 5;
    int screenWidth = 800;
    
    // 测试1: 向左撞墙（球在左边界内）
    float x1 = 5, speedX1 = -5;  // x=5 时，x-radius=0，正好碰墙
    printf("测试1: 向左撞墙 (x=%.1f, speed=%.1f)\n", x1, speedX1);
    bounceEdge(x1, speedX1, radius, screenWidth);
    if (speedX1 > 0 && x1 == radius) {
        printf("  ✅ 通过\n");
        passed++;
    } else {
        printf("  ❌ 失败: 速度=%.1f (应为正), 位置=%.1f (应为%.1f)\n", speedX1, x1, radius);
        failed++;
    }
    
    // 测试2: 向右撞墙
    float x2 = 795, speedX2 = 5;  // x=795, radius=5, x+radius=800，正好碰墙
    printf("\n测试2: 向右撞墙 (x=%.1f, speed=%.1f)\n", x2, speedX2);
    bounceEdge(x2, speedX2, radius, screenWidth);
    if (speedX2 < 0 && x2 == screenWidth - radius) {
        printf("  ✅ 通过\n");
        passed++;
    } else {
        printf("  ❌ 失败: 速度=%.1f (应为负), 位置=%.1f (应为%.1f)\n", 
               speedX2, x2, screenWidth - radius);
        failed++;
    }
    
    // 测试3: 不撞墙
    float x3 = 400, speedX3 = 5;
    printf("\n测试3: 不撞墙 (x=%.1f, speed=%.1f)\n", x3, speedX3);
    bounceEdge(x3, speedX3, radius, screenWidth);
    if (speedX3 == 5 && x3 == 400) {
        printf("  ✅ 通过\n");
        passed++;
    } else {
        printf("  ❌ 失败: 速度=%.1f, 位置=%.1f\n", speedX3, x3);
        failed++;
    }
    
    printf("\n====================================\n");
    printf("结果: %d 通过, %d 失败\n", passed, failed);
    
    return failed;
}
