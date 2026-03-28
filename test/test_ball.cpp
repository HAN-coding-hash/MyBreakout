#include <cstdio>
#include <cmath>
#include "../src/Ball.h"

// 简单的断言宏
#define ASSERT(condition, message) \
    if (!(condition)) { \
        printf("❌ FAILED: %s\n", message); \
        return 1; \
    } else { \
        printf("✅ PASS: %s\n", message); \
    }

int test_ball_initialization() {
    Ball ball(400, 500, 10);
    
    ASSERT(ball.GetPosition().x == 400, "Ball X position initial");
    ASSERT(ball.GetPosition().y == 500, "Ball Y position initial");
    ASSERT(ball.GetRadius() == 10, "Ball radius");
    ASSERT(!ball.IsLaunched(), "Ball not launched initially");
    
    return 0;
}

int test_ball_launch() {
    Ball ball(400, 500, 10);
    
    ball.Launch(400);
    
    ASSERT(ball.IsLaunched(), "Ball launched after Launch()");
    
    Vector2 speed = ball.GetSpeed();
    // 发射后速度不为零
    ASSERT(speed.x != 0 || speed.y != 0, "Ball has speed after launch");
    
    return 0;
}

int test_ball_boundary_bounce() {
    Ball ball(10, 10, 5);
    ball.Launch(400);
    
    // 设置向左的速度
    ball.SetSpeed({-5, 0});
    float speedX = ball.GetSpeed().x;
    
    // 模拟边界反弹（左边界）
    ball.BounceEdge(800, 600);
    
    // 应该在边界处反弹，X速度变正
    ASSERT(ball.GetSpeed().x > 0, "Ball bounces off left wall");
    
    return 0;
}

int test_ball_paddle_collision_detection() {
    Ball ball(400, 540, 10);
    ball.Launch(400);
    // 设置向下运动
    ball.SetSpeed({0, 5});
    
    Rectangle paddle = {340, 550, 120, 15};
    
    // 检测碰撞条件（简单模拟）
    Vector2 pos = ball.GetPosition();
    float radius = ball.GetRadius();
    
    bool collision = (pos.y + radius >= paddle.y) &&
                     (pos.x + radius >= paddle.x) &&
                     (pos.x - radius <= paddle.x + paddle.width);
    
    ASSERT(collision, "Ball collides with paddle when positioned above");
    
    return 0;
}

int main() {
    printf("\n========== Ball 类单元测试 ==========\n\n");
    
    int failed = 0;
    
    failed += test_ball_initialization();
    printf("\n");
    
    failed += test_ball_launch();
    printf("\n");
    
    failed += test_ball_boundary_bounce();
    printf("\n");
    
    failed += test_ball_paddle_collision_detection();
    printf("\n");
    
    printf("====================================\n");
    if (failed == 0) {
        printf("🎉 所有测试通过！\n");
    } else {
        printf("❌ 有 %d 个测试失败\n", failed);
    }
    
    return failed;
}
