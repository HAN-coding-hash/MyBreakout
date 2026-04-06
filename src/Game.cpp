#include "Game.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>

Game::Game() 
    : screenWidth(800), screenHeight(600),
      ball(400, 530, 10),
      paddle(340, 550, 120, 15),
      currentState(GameState::MENU),  // 初始状态为 MENU
      score(0), lives(3), winCount(0), gameTime(0.0f) {
    
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
    // 移动挡板（所有状态下都可以移动？只在 MENU 和 PLAYING 时移动）
    if (currentState == GameState::MENU || currentState == GameState::PLAYING) {
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) paddle.MoveLeft();
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) paddle.MoveRight();
    }
    
    // 根据当前状态处理输入
    switch (currentState) {
        case GameState::MENU:
            // 按空格发射球，进入 PLAYING 状态
            if (IsKeyPressed(KEY_SPACE)) {
                ball.Launch(paddle.GetRect().x + paddle.GetRect().width / 2);
                currentState = GameState::PLAYING;
            }
            break;
            
        case GameState::PLAYING:
            // 按 P 暂停
            if (IsKeyPressed(KEY_P)) {
                currentState = GameState::PAUSED;
            }
            break;
            
        case GameState::PAUSED:
            // 按 P 继续游戏
            if (IsKeyPressed(KEY_P)) {
                currentState = GameState::PLAYING;
            }
            break;
            
        case GameState::GAME_OVER:
        case GameState::VICTORY:
            // 按 R 重置游戏
            if (IsKeyPressed(KEY_R)) {
                // 重置所有状态
                score = 0;
                lives = 3;
                gameTime = 0.0f;
                currentState = GameState::MENU;
                
                // 重置砖块
                InitBricks();
                RandomGoldenBrick();
                
                // 重置球
                ball.ResetToPaddle(paddle.GetRect().x + paddle.GetRect().width / 2, paddle.GetRect().y);
            }
            break;
    }
}

void Game::Update() {
    // 只在 PLAYING 状态更新游戏逻辑
    if (currentState != GameState::PLAYING) return;
    if (!ball.IsLaunched()) return;
    
    // 更新游戏时间
    gameTime += GetFrameTime();
    
    // 更新物理
    ball.Move();
    ball.BounceEdge(screenWidth, screenHeight);
    
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
            currentState = GameState::GAME_OVER;
        } else {
            ball.ResetToPaddle(paddle.GetRect().x + paddle.GetRect().width / 2, paddle.GetRect().y);
            currentState = GameState::MENU;  // 回到 MENU 等待发射
        }
    }
}

void Game::CheckGameOver() {
    if (winCount <= 0) {
        currentState = GameState::VICTORY;
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(WHITE);
    
    // 绘制砖块
    for (auto& brick : bricks) brick.Draw();
    
    paddle.Draw();
    ball.Draw();
    
    // UI - 分数和生命
    DrawText(TextFormat("Score: %d", score), 10, 10, 20, DARKGRAY);
    DrawText(TextFormat("Lives: %d", lives), 10, 40, 20, DARKGRAY);
    
    // 根据状态显示不同提示
    switch (currentState) {
        case GameState::MENU:
            if (!ball.IsLaunched()) {
                DrawText("Press SPACE to launch", screenWidth/2 - 120, screenHeight/2 + 100, 20, DARKGRAY);
            }
            break;
            
        case GameState::PAUSED:
            DrawText("PAUSED", screenWidth/2 - 40, screenHeight/2, 30, RED);
            DrawText("Press P to resume", screenWidth/2 - 70, screenHeight/2 + 40, 20, DARKGRAY);
            break;
            
        case GameState::GAME_OVER:
            DrawText("GAME OVER", screenWidth/2 - 60, screenHeight/2, 30, RED);
            DrawText("Press R to restart", screenWidth/2 - 70, screenHeight/2 + 40, 20, DARKGRAY);
            break;
            
        case GameState::VICTORY:
            DrawText("YOU WIN!", screenWidth/2 - 50, screenHeight/2, 30, GREEN);
            DrawText("Press R to restart", screenWidth/2 - 70, screenHeight/2 + 40, 20, DARKGRAY);
            break;
            
        case GameState::PLAYING:
            // 游戏中不显示额外提示
            break;
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
