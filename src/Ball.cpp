#include "Ball.h"
#include <cmath>
#include <cstdlib>

Ball::Ball(float x, float y, float r) {
    position = {x, y};
    speed = {0, 0};
    radius = r;
    launched = false;
}

void Ball::Move() {
    if (!launched) return;
    position.x += speed.x;
    position.y += speed.y;
}

#ifndef TEST_MODE
void Ball::Draw() {
    DrawCircleV(position, radius, RED);
    if (!launched) {
        DrawText("PRESS SPACE", (int)position.x - 55, (int)position.y - 30, 16, DARKGRAY);
    }
}
#endif

void Ball::BounceEdge(int screenWidth, int screenHeight) {
    if (!launched) return;
    
    // 左边界
    if (position.x - radius <= 0) {
        position.x = radius;
        speed.x = std::abs(speed.x);
    }
    // 右边界
    if (position.x + radius >= screenWidth) {
        position.x = screenWidth - radius;
        speed.x = -std::abs(speed.x);
    }
    // 上边界
    if (position.y - radius <= 0) {
        position.y = radius;
        speed.y = std::abs(speed.y);
    }
}

void Ball::Launch(float paddleX) {
    if (launched) return;
    speed = {4.0f, -5.0f};
    launched = true;
    position.x = paddleX;
}

void Ball::ResetToPaddle(float paddleX, float paddleY) {
    position = {paddleX, paddleY - radius - 5};
    speed = {0, 0};
    launched = false;
}