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
        
        // 未发射时，球跟随挡板移动
        if (!ball.IsLaunched()) {
            float paddleCenterX = paddle.GetRect().x + paddle.GetRect().width / 2;
            ball.ResetToPaddle(paddleCenterX, paddle.GetRect().y);
        }
        
        // 发射球
        if (!ball.IsLaunched() && IsKeyPressed(KEY_SPACE)) {
            ball.Launch(paddle.GetRect().x + paddle.GetRect().width / 2);
        }
        
        if (!gameOver && ball.IsLaunched()) {
            ball.Move();
            ball.BounceEdge(screenWidth, screenHeight);
            
                       // ========== 挡板碰撞 ==========
            Rectangle paddleRect = paddle.GetRect();
            Vector2 ballPos = ball.GetPosition();
            Vector2 ballSpeed = ball.GetSpeed();
            float ballRadius = ball.GetRadius();
            
            // 检查球是否碰到挡板（只处理向下运动的球）
            if (ballSpeed.y > 0 &&
                ballPos.y + ballRadius >= paddleRect.y &&
                ballPos.y + ballRadius <= paddleRect.y + paddleRect.height + ballSpeed.y &&
                ballPos.x + ballRadius >= paddleRect.x &&
                ballPos.x - ballRadius <= paddleRect.x + paddleRect.width) {
                
                // 计算击中点偏移（-1到1）
                float hitPoint = (ballPos.x - (paddleRect.x + paddleRect.width/2)) / (paddleRect.width/2);
                if (hitPoint < -1) hitPoint = -1;
                if (hitPoint > 1) hitPoint = 1;
                
                // 根据击中点计算反弹角度（-60° 到 60°）
                float angle = hitPoint * 60.0f;
                float rad = angle * 3.14159f / 180.0f;
                
                // 保持速度大小，向上反弹
                float speedMagnitude = sqrt(ballSpeed.x * ballSpeed.x + ballSpeed.y * ballSpeed.y);
                if (speedMagnitude < 8) speedMagnitude = 8;
                if (speedMagnitude > 15) speedMagnitude = 15;
                
                float newSpeedX = speedMagnitude * sin(rad);
                float newSpeedY = -speedMagnitude * cos(rad);
                
                // 设置新速度
                ball.SetSpeed({newSpeedX, newSpeedY});
                
                // 修正位置到挡板上面
                ball.SetPosition({ballPos.x, paddleRect.y - ballRadius});
                // 确保球处于发射状态
                ball.SetLaunched(true);
            }
            
            // ========== 砖块碰撞 ==========
            for (auto& brick : bricks) {
                if (brick.IsActive()) {
                    Rectangle brickRect = brick.GetRect();
                    
                    if (CheckCollisionCircleRec(ball.GetPosition(), ball.GetRadius(), brickRect)) {
                        brick.SetActive(false);
                        
                        // 计算碰撞方向
                        Vector2 currentSpeed = ball.GetSpeed();
                        float ballCenterX = ball.GetPosition().x;
                        float ballCenterY = ball.GetPosition().y;
                        float brickCenterX = brickRect.x + brickRect.width/2;
                        float brickCenterY = brickRect.y + brickRect.height/2;
                        
                        float dx = ballCenterX - brickCenterX;
                        float dy = ballCenterY - brickCenterY;
                        
                        // 根据重叠程度决定反弹方向
                        if (fabs(dx) > fabs(dy)) {
                            // 水平碰撞
                            ball.SetSpeed({-currentSpeed.x, currentSpeed.y});
                        } else {
                            // 垂直碰撞
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
            
            // ========== 球掉底 ==========
            if (ball.GetPosition().y + ball.GetRadius() > screenHeight) {
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
        
        // ========== 绘制 ==========
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