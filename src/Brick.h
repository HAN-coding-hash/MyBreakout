#ifndef BRICK_H
#define BRICK_H
#include "raylib.h"

class Brick {
private:
    Rectangle rect;
    bool active;
    bool isGolden;

public:
    Brick(float x, float y, float width, float height);
    void Draw();
    void SetGolden(bool golden) { isGolden = golden; }
    bool IsGolden() { return isGolden; }
    bool IsActive() { return active; }
    void SetActive(bool a) { active = a; }
    Rectangle GetRect() { return rect; }
};

#endif
