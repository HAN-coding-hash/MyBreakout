#ifndef BALL_H
#define BALL_H
#include "raylib.h"

class Ball {
private:
    Vector2 position;
    Vector2 speed;
    float radius;
    bool launched;

public:
    Ball(float x, float y, float r);
    void Move();
    void Draw();
    void BounceEdge(int screenWidth, int screenHeight);
    void Launch(float paddleX);
    void ResetToPaddle(float paddleX, float paddleY);
    void SetSpeed(Vector2 sp) { speed = sp; }
    Vector2 GetSpeed() { return speed; }
    bool IsLaunched() { return launched; }
    Vector2 GetPosition() { return position; }
    float GetRadius() { return radius; }
};

#endif