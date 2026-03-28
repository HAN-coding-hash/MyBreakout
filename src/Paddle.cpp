#include "Paddle.h"

Paddle::Paddle(float x, float y, float width, float height) {
    rect = {x, y, width, height};
    speed = 8.0f;
}

void Paddle::MoveLeft() {
    rect.x -= speed;
    if (rect.x < 0) rect.x = 0;
}

void Paddle::MoveRight() {
    rect.x += speed;
    if (rect.x + rect.width > 800) rect.x = 800 - rect.width;
}

void Paddle::Draw() {
    DrawRectangleRec(rect, DARKBLUE);
}
