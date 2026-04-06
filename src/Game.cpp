#include "Game.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>

Game::Game() 
    : screenWidth(800), screenHeight(600),
      ball(400, 530, 10),
      paddle(340, 550, 120, 15),
      score(0), lives(3), winCount(0),
      gameOver(false), paused(false), victory(false), gameTime(0.0f) {
    
    // 初始化随机种子
    srand((unsigned int)time(nullptr));
    
    // 初始化砖块
    InitBricks();
    RandomGoldenBrick();
    
    // 初始化窗口
    InitWindow(screenWidth, screenHeight, "MyBreakout");
    SetTargetFPS(60);
}

Game::~Game() {
    CloseWindow();
}

void Game::InitBricks() {
    bricks.clear();
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 8; col++) {
            bricks.emplace_back(50 + col * 95, 80 + row * 35, 85, 25);
        }
    }
    winCount = bricks.size();
}

void Game::RandomGoldenBrick() {
    if (!bricks.empty()) {
        int goldenIndex = rand() % bricks.size();
        bricks[goldenIndex].SetGolden(true);
        printf("金砖位置: 行%d 列%d\n", goldenIndex / 8, goldenIndex % 8);
    }
}

void Game::HandleInput() {
    // 移动挡板
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) paddle.MoveLeft();
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) paddle.MoveRight();
    
    // 发射球
    if (!ball.IsLaunched() && IsKeyPressed(KEY_SPACE)) {
        ball.Launch(paddle.GetRect().x + paddle.GetRect().width / 2);
    }
    
    // 暂停
    if (IsKeyPressed(KEY_P) && !gameOver) {
        paused = !paused;
    }
    
    // 重置
    if (IsKeyPressed(KEY_R) && gameOver) {
        score = 0;
        lives = 3;
        gameOver = false;
        victory = false;
        gameTime = 0.0f;
        InitBricks();
        RandomGoldenBrick();
        ball.ResetToPaddle(paddle.GetRect().x + paddle.GetRect().width / 2, paddle.GetRect().y);
    }
}

void Game::Update() {
    if (gameOver || paused) return;
    if (!ball.IsLaunched()) return;
    
    // 更新游戏时间
    gameTime += GetFrameTime();
    
    // 更新物理
    ball.Move();
    ball.BounceEdge(screenWidth, screenHeight);
    
    // 未发射时球跟随挡板
    if (!ball.IsLaunched()) {
        float paddleCenterX = paddle.GetRect().x + paddle.GetRect().width / 2;
        ball.ResetToPaddle(paddleCenterX, paddle.GetRect().y);
    }
    
    CheckCollisions();
    CheckGameOver();
}

void Game::CheckCollisions() {
    // 挡板碰撞
    Rectangle paddleRect = paddle.GetRect();
    Vector2 ballPos = ball.GetPosition();
    Vector2 ballSpeed = ball.GetSpeed();
    float ballRadius = ball.GetRadius();
    
    if (ballSpeed.y > 0 &&
        ballPos.y + ballRadius >= paddleRect.y &&
        ballPos.y + ballRadius <= paddleRect.y + paddleRect.height + ballSpeed.y &&
        ballPos.x + ballRadius >= paddleRect.x &&
        ballPos.x - ballRadius <= paddleRect.x + paddleRect.width) {
        
        // 计算击中点偏移
        float hitPoint = (ballPos.x - (paddleRect.x + paddleRect.width/2)) / (paddleRect.width/2);
        if (hitPoint < -1) hitPoint = -1;
        if (hitPoint > 1) hitPoint = 1;
        
        // 计算反弹角度
        float angle = hitPoint * 60.0f;
        float rad = angle * 3.14159f / 180.0f;
        
        float speedMagnitude = sqrt(ballSpeed.x * ballSpeed.x + ballSpeed.y * ballSpeed.y);
        if (speedMagnitude < 8) speedMagnitude = 8;
        if (speedMagnitude > 15) speedMagnitude = 15;
        
        ball.SetSpeed({speedMagnitude * sin(rad), -speedMagnitude * cos(rad)});
        ball.SetPosition({ballPos.x, paddleRect.y - ballRadius});
        ball.SetLaunched(true);
    }
    
    // 砖块碰撞
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
                
                if (fabs(ballCenterX - brickCenterX) > fabs(ballCenterY - brickCenterY)) {
                    ball.SetSpeed({-currentSpeed.x, currentSpeed.y});
                } else {
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
    if (ball.GetPosition().y + ball.GetRadius() > screenHeight) {
        lives--;
        if (lives <= 0) {
            gameOver = true;
        } else {
            ball.ResetToPaddle(paddle.GetRect().x + paddle.GetRect().width / 2, paddle.GetRect().y);
        }
    }
}

void Game::CheckGameOver() {
    if (winCount <= 0) {
        gameOver = true;
        victory = true;
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(WHITE);
    
    // 绘制砖块
    for (auto& brick : bricks) brick.Draw();
    
    paddle.Draw();
    ball.Draw();
    
    // UI
    DrawText(TextFormat("Score: %d", score), 10, 10, 20, DARKGRAY);
    DrawText(TextFormat("Lives: %d", lives), 10, 40, 20, DARKGRAY);
    
    if (!ball.IsLaunched()) {
        DrawText("Press SPACE to launch", screenWidth/2 - 120, screenHeight/2 + 100, 20, DARKGRAY);
    }
    
    if (paused) {
        DrawText("PAUSED", screenWidth/2 - 40, screenHeight/2, 30, RED);
        DrawText("Press P to resume", screenWidth/2 - 70, screenHeight/2 + 40, 20, DARKGRAY);
    }
    
    if (gameOver) {
        DrawText(victory ? "YOU WIN!" : "GAME OVER", screenWidth/2 - 60, screenHeight/2, 30, RED);
        DrawText("Press R to restart", screenWidth/2 - 70, screenHeight/2 + 40, 20, DARKGRAY);
    }
    
    EndDrawing();
}

void Game::Run() {
    while (!WindowShouldClose()) {
        HandleInput();
        Update();
        Draw();
    }
}
