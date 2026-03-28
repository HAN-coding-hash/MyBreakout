#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cmath>

int main() {
    srand((unsigned int)time(nullptr));
    
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "MyBreakout");
    SetTargetFPS(60);
    
    Ball ball(400, 530, 10);
    Paddle paddle(340, 550, 120, 15);
    
    std::vector<Brick> bricks;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 8; col++) {
            bricks.emplace_back(50 + col * 95, 80 + row * 35, 85, 25);
        }
    }
    
    if (!bricks.empty()) {
        int goldenIndex = rand() % bricks.size();
        bricks[goldenIndex].SetGolden(true);
        printf("金砖位置: 行%d 列%d\n", goldenIndex / 8, goldenIndex % 8);
    }
    
    int score = 0;
    int lives = 3;
    int winCount = bricks.size();
    bool gameOver = false;
    
    while (!WindowShouldClose()) {
        // 输入处理
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) paddle.MoveLeft();
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) paddle.MoveRight();
        
        if (!ball.IsLaunched() && IsKeyPressed(KEY_SPACE)) {
            ball.Launch(paddle.GetRect().x + paddle.GetRect().width / 2);
        }
        
        if (!gameOver) {
            ball.Move();
            ball.BounceEdge(screenWidth, screenHeight);
            
            // 挡板碰撞（正确版本）
            Rectangle paddleRect = paddle.GetRect();
            if (ball.GetPosition().y + ball.GetRadius() >= paddleRect.y &&
                ball.GetPosition().y - ball.GetRadius() <= paddleRect.y + paddleRect.height &&
                ball.GetPosition().x + ball.GetRadius() >= paddleRect.x &&
                ball.GetPosition().x - ball.GetRadius() <= paddleRect.x + paddleRect.width) {
                
                // 计算击中点偏移（-1到1）
                float hitPoint = (ball.GetPosition().x - (paddleRect.x + paddleRect.width/2)) / (paddleRect.width/2);
                if (hitPoint < -1) hitPoint = -1;
                if (hitPoint > 1) hitPoint = 1;
                
                // 根据击中点计算反弹角度
                float angle = hitPoint * 60.0f;  // -60° 到 60°
                float rad = angle * 3.14159f / 180.0f;
                
                // 保持速度大小，改变方向
                Vector2 currentSpeed = ball.GetSpeed();
                float speedMagnitude = sqrt(currentSpeed.x * currentSpeed.x + currentSpeed.y * currentSpeed.y);
                if (speedMagnitude < 5) speedMagnitude = 5;
                
                ball.SetSpeed({speedMagnitude * sin(rad), -speedMagnitude * cos(rad)});
                
                // 修正位置，避免卡在挡板里
                ball.ResetToPaddle(paddleRect.x + paddleRect.width / 2, paddleRect.y);
            }
            
            // 砖块碰撞（正确版本）
            for (auto& brick : bricks) {
                if (brick.IsActive()) {
                    Rectangle brickRect = brick.GetRect();
                    
                    // 检测碰撞
                    if (CheckCollisionCircleRec(ball.GetPosition(), ball.GetRadius(), brickRect)) {
                        brick.SetActive(false);
                        
                        // 计算碰撞方向
                        float overlapLeft = (ball.GetPosition().x + ball.GetRadius()) - brickRect.x;
                        float overlapRight = (brickRect.x + brickRect.width) - (ball.GetPosition().x - ball.GetRadius());
                        float overlapTop = (ball.GetPosition().y + ball.GetRadius()) - brickRect.y;
                        float overlapBottom = (brickRect.y + brickRect.height) - (ball.GetPosition().y - ball.GetRadius());
                        
                        // 找到最小的重叠量，决定碰撞方向
                        float minOverlap = overlapLeft;
                        int direction = 0; // 0:左, 1:右, 2:上, 3:下
                        
                        if (overlapRight < minOverlap) { minOverlap = overlapRight; direction = 1; }
                        if (overlapTop < minOverlap) { minOverlap = overlapTop; direction = 2; }
                        if (overlapBottom < minOverlap) { minOverlap = overlapBottom; direction = 3; }
                        
                        // 根据方向反弹
                        Vector2 currentSpeed = ball.GetSpeed();
                        if (direction == 0 || direction == 1) {
                            // 左右碰撞：X速度反向
                            ball.SetSpeed({-currentSpeed.x, currentSpeed.y});
                        } else {
                            // 上下碰撞：Y速度反向
                            ball.SetSpeed({currentSpeed.x, -currentSpeed.y});
                        }
                        
                        // 加分
                        if (brick.IsGolden()) {
                            score += 10000;
                            printf("★★★ 金砖！+10000分！★★★\n");
                        } else {
                            score += 10;
                        }
                        
                        winCount--;
                        break;
                    }
                }
            }
            
            // 球掉底
            if (ball.GetPosition().y > screenHeight) {
                lives--;
                if (lives <= 0) {
                    gameOver = true;
                } else {
                    ball.ResetToPaddle(paddle.GetRect().x + paddle.GetRect().width / 2, paddle.GetRect().y);
                }
            }
            
            // 胜利
            if (winCount <= 0) {
                gameOver = true;
            }
        }
        
        // 绘制
        BeginDrawing();
        ClearBackground(WHITE);
        
        for (auto& brick : bricks) brick.Draw();
        paddle.Draw();
        ball.Draw();
        
        DrawText(TextFormat("Score: %d", score), 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Lives: %d", lives), 10, 40, 20, DARKGRAY);
        
        if (!ball.IsLaunched()) {
            DrawText("Press SPACE to launch", screenWidth/2 - 120, screenHeight/2 + 100, 20, DARKGRAY);
        }
        
        if (gameOver) {
            DrawText(winCount <= 0 ? "YOU WIN!" : "GAME OVER", screenWidth/2 - 60, screenHeight/2, 30, RED);
            DrawText("Press R to restart", screenWidth/2 - 70, screenHeight/2 + 40, 20, DARKGRAY);
            
            if (IsKeyPressed(KEY_R)) {
                score = 0;
                lives = 3;
                gameOver = false;
                
                bricks.clear();
                for (int row = 0; row < 5; row++) {
                    for (int col = 0; col < 8; col++) {
                        bricks.emplace_back(50 + col * 95, 80 + row * 35, 85, 25);
                    }
                }
                int goldenIndex = rand() % bricks.size();
                bricks[goldenIndex].SetGolden(true);
                winCount = bricks.size();
                
                ball.ResetToPaddle(paddle.GetRect().x + paddle.GetRect().width / 2, paddle.GetRect().y);
            }
        }
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}