#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio> 

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
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) paddle.MoveLeft();
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) paddle.MoveRight();
        
        if (!ball.IsLaunched() && IsKeyPressed(KEY_SPACE)) {
            ball.Launch(paddle.GetRect().x + paddle.GetRect().width / 2);
        }
        
        if (!gameOver) {
            ball.Move();
            ball.BounceEdge(screenWidth, screenHeight);
            
            Rectangle paddleRect = paddle.GetRect();
            if (ball.GetPosition().y + ball.GetRadius() >= paddleRect.y &&
                ball.GetPosition().x >= paddleRect.x - ball.GetRadius() &&
                ball.GetPosition().x <= paddleRect.x + paddleRect.width + ball.GetRadius()) {
                ball.Launch(paddleRect.x + paddleRect.width / 2);
            }
            
            for (auto& brick : bricks) {
                if (brick.IsActive() && CheckCollisionCircleRec(ball.GetPosition(), ball.GetRadius(), brick.GetRect())) {
                    brick.SetActive(false);
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
            
            if (ball.GetPosition().y > screenHeight) {
                lives--;
                if (lives <= 0) {
                    gameOver = true;
                } else {
                    ball.ResetToPaddle(paddleRect.x + paddleRect.width / 2, paddleRect.y);
                }
            }
            
            if (winCount <= 0) {
                gameOver = true;
            }
        }
        
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
