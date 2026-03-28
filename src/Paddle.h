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
    void MoveRight();
    void Draw();
    Rectangle GetRect() { return rect; }
};

#endif
