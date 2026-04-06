#ifndef PADDLE_H
#define PADDLE_H
#include "raylib.h"

class Paddle {
private:
    Rectangle rect;
    float speed;

public:
    Paddle(float x, float y, float width, float height);
    void MoveLeft();
    void MoveRight(int screenWidth);  // 添加屏幕宽度参数
    void Draw();
    Rectangle GetRect() const { return rect; }
    void SetX(float x) { rect.x = x; }
    float GetSpeed() const { return speed; }
    void SetSpeed(float s) { speed = s; }
};

#endif
