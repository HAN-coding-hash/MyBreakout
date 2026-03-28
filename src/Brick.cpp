#include "Brick.h"

Brick::Brick(float x, float y, float width, float height) {
    rect = {x, y, width, height};
    active = true;
    isGolden = false;
}

void Brick::Draw() {
    if (!active) return;
    
    if (isGolden) {
        DrawRectangleRec(rect, GOLD);
        Vector2 center = {rect.x + rect.width/2, rect.y + rect.height/2};
        DrawText("★", (int)center.x - 5, (int)center.y - 8, 16, YELLOW);
    } else {
        DrawRectangleRec(rect, BLUE);
    }
    DrawRectangleLinesEx(rect, 1, DARKBLUE);
}
