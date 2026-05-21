#include "Game.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

Game::Game()
    : screenWidth(800), screenHeight(600),
      ball(400, 530, 10),
      paddle(340, 550, 120, 15),
      currentMode(PowerUpMode::NONE),
      modeTimer(0.0f),
      currentState(GameState::MENU),
      score(0), lives(3), winCount(0), gameTime(0.0f),
      originalBallSpeed(8.0f), originalBallRadius(10.0f),
      originalPaddleWidth(120.0f) {

    srand((unsigned int)time(nullptr));
    
    // 初始化砖块
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 8; col++) {
            bricks.emplace_back(50 + col * 95, 80 + row * 35, 85, 25);
        }
    }
    winCount = bricks.size();
    
    // 随机金砖
    if (!bricks.empty()) {
        int goldenIndex = rand() % bricks.size();
        bricks[goldenIndex].SetGolden(true);
        printf("金砖位置: 行%d 列%d\n", goldenIndex / 8, goldenIndex % 8);
    }

    InitWindow(screenWidth, screenHeight, "MyBreakout - PowerUp Modes");
    SetTargetFPS(60);
}

Game::~Game() {
    CloseWindow();
}

void Game::HandleInput() {
    // 移动挡板
    if (currentState == GameState::MENU || currentState == GameState::PLAYING) {
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) paddle.MoveLeft();
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) paddle.MoveRight(screenWidth);
    }

    // 未发射时，球跟随挡板移动
    if (!ball.IsLaunched()) {
        float paddleCenterX = paddle.GetRect().x + paddle.GetRect().width / 2;
        ball.ResetToPaddle(paddleCenterX, paddle.GetRect().y);
    }

    // 模式选择
    if (currentState == GameState::PLAYING) {
        if (IsKeyPressed(KEY_ONE)) {
            currentMode = PowerUpMode::SPLIT;
            modeTimer = 10.0f;
            printf("★★★ 分裂模式激活！★★★\n");
        }
        if (IsKeyPressed(KEY_TWO)) {
            currentMode = PowerUpMode::SLOW_TIME;
            modeTimer = 10.0f;
            // 减速
            Vector2 sp = ball.GetSpeed();
            ball.SetSpeed({sp.x * 0.5f, sp.y * 0.5f});
            printf("★★★ 时停模式激活！★★★\n");
        }
        if (IsKeyPressed(KEY_THREE)) {
            currentMode = PowerUpMode::SWEEP;
            modeTimer = 10.0f;
            ball.SetRadius(originalBallRadius * 2);
            printf("★★★ 横扫模式激活！★★★\n");
        }
    }

    // 模式计时
    if (currentMode != PowerUpMode::NONE) {
        modeTimer -= GetFrameTime();
        if (modeTimer <= 0) {
            if (currentMode == PowerUpMode::SLOW_TIME) {
                Vector2 sp = ball.GetSpeed();
                ball.SetSpeed({sp.x * 2.0f, sp.y * 2.0f});
            }
            if (currentMode == PowerUpMode::SWEEP) {
                ball.SetRadius(originalBallRadius);
            }
            if (currentMode == PowerUpMode::SPLIT) {
                extraBalls.clear();
            }
            currentMode = PowerUpMode::NONE;
            printf("模式效果结束\n");
        }
    }

    switch (currentState) {
        case GameState::MENU:
            if (IsKeyPressed(KEY_SPACE)) {
                ball.Launch(paddle.GetRect().x + paddle.GetRect().width / 2);
                currentState = GameState::PLAYING;
            }
            break;
        case GameState::PLAYING:
            if (IsKeyPressed(KEY_P)) currentState = GameState::PAUSED;
            break;
        case GameState::PAUSED:
            if (IsKeyPressed(KEY_P)) currentState = GameState::PLAYING;
            break;
        case GameState::GAME_OVER:
        case GameState::VICTORY:
            if (IsKeyPressed(KEY_R)) {
                score = 0;
                lives = 3;
                gameTime = 0.0f;
                currentState = GameState::MENU;
                currentMode = PowerUpMode::NONE;
                extraBalls.clear();
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
                ball.SetRadius(originalBallRadius);
            }
            break;
    }
}

void Game::Update() {
    if (currentState != GameState::PLAYING) return;
    if (!ball.IsLaunched()) return;

    gameTime += GetFrameTime();
    ball.Move();
    ball.BounceEdge(screenWidth, screenHeight);

    // 更新额外球
    for (auto& extraBall : extraBalls) {
        extraBall.Move();
        extraBall.BounceEdge(screenWidth, screenHeight);
    }

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

        float hitPoint = (ballPos.x - (paddleRect.x + paddleRect.width/2)) / (paddleRect.width/2);
        if (hitPoint < -1) hitPoint = -1;
        if (hitPoint > 1) hitPoint = 1;

        float angle = hitPoint * 60.0f;
        float rad = angle * 3.14159f / 180.0f;

        float speedMagnitude = sqrt(ballSpeed.x * ballSpeed.x + ballSpeed.y * ballSpeed.y);
        if (speedMagnitude < 8) speedMagnitude = 8;
        if (speedMagnitude > 15) speedMagnitude = 15;

        float newSpeedX = speedMagnitude * sin(rad);
        float newSpeedY = -speedMagnitude * cos(rad);

        ball.SetSpeed({newSpeedX, newSpeedY});
        ball.SetPosition({ballPos.x, paddleRect.y - ballRadius});
        ball.SetLaunched(true);

        // 分裂模式
        if (currentMode == PowerUpMode::SPLIT && extraBalls.empty()) {
            Ball newBall(ballPos.x, paddleRect.y - ballRadius, originalBallRadius);
            newBall.SetSpeed({-newSpeedX, newSpeedY});
            newBall.SetLaunched(true);
            extraBalls.push_back(newBall);
            printf("★★★ 球分裂！★★★\n");
        }
    }

    // 砖块碰撞 - 主球
    for (auto& brick : bricks) {
        if (brick.IsActive()) {
            Rectangle brickRect = brick.GetRect();
            // 碰撞检测
            if (CheckCollisionCircleRec(ball.GetPosition(), ball.GetRadius(), brickRect)) {
                brick.SetActive(false);
                
                // 计算反弹方向
                Vector2 currentSpeed = ball.GetSpeed();
                float ballCenterX = ball.GetPosition().x;
                float ballCenterY = ball.GetPosition().y;
                float brickCenterX = brickRect.x + brickRect.width/2;
                float brickCenterY = brickRect.y + brickRect.height/2;
                
                float overlapLeft = (ballCenterX + ballRadius) - brickRect.x;
                float overlapRight = (brickRect.x + brickRect.width) - (ballCenterX - ballRadius);
                float overlapTop = (ballCenterY + ballRadius) - brickRect.y;
                float overlapBottom = (brickRect.y + brickRect.height) - (ballCenterY - ballRadius);
                
                float minOverlap = overlapLeft;
                int direction = 0;
                if (overlapRight < minOverlap) { minOverlap = overlapRight; direction = 1; }
                if (overlapTop < minOverlap) { minOverlap = overlapTop; direction = 2; }
                if (overlapBottom < minOverlap) { minOverlap = overlapBottom; direction = 3; }
                
                if (direction == 0 || direction == 1) {
                    ball.SetSpeed({-currentSpeed.x, currentSpeed.y});
                } else {
                    ball.SetSpeed({currentSpeed.x, -currentSpeed.y});
                }
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

    // 砖块碰撞 - 额外球
    for (auto& extraBall : extraBalls) {
        for (auto& brick : bricks) {
            if (brick.IsActive() && CheckCollisionCircleRec(extraBall.GetPosition(), extraBall.GetRadius(), brick.GetRect())) {
                brick.SetActive(false);
                if (brick.IsGolden()) {
                    score += 10000;
                } else {
                    score += 10;
                }
                winCount--;
                break;
            }
        }
    }

    // 移除飞出屏幕的额外球
    extraBalls.erase(std::remove_if(extraBalls.begin(), extraBalls.end(),
        [screenHeight = screenHeight](Ball& b) {
            return b.GetPosition().y + b.GetRadius() > screenHeight;
        }), extraBalls.end());

    // 主球掉底
    if (ball.GetPosition().y + ball.GetRadius() > screenHeight) {
        lives--;
        if (lives <= 0) {
            currentState = GameState::GAME_OVER;
        } else {
            ball.ResetToPaddle(paddle.GetRect().x + paddle.GetRect().width / 2, paddle.GetRect().y);
            currentState = GameState::MENU;
            extraBalls.clear();
            currentMode = PowerUpMode::NONE;
            ball.SetRadius(originalBallRadius);
        }
    }

    if (winCount <= 0) {
        currentState = GameState::VICTORY;
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(WHITE);

    for (auto& brick : bricks) brick.Draw();
    paddle.Draw();
    ball.Draw();
    for (auto& extraBall : extraBalls) extraBall.Draw();

    DrawText(TextFormat("Score: %d", score), 10, 10, 20, DARKGRAY);
    DrawText(TextFormat("Lives: %d", lives), 10, 40, 20, DARKGRAY);

    if (currentMode != PowerUpMode::NONE) {
        const char* modeText = "";
        switch (currentMode) {
            case PowerUpMode::SPLIT: modeText = "SPLIT"; break;
            case PowerUpMode::SLOW_TIME: modeText = "SLOW TIME"; break;
            case PowerUpMode::SWEEP: modeText = "SWEEP"; break;
            default: break;
        }
        DrawText(TextFormat("MODE: %s %.1fs", modeText, modeTimer), screenWidth - 180, 10, 16, GREEN);
    }

    if (currentState == GameState::PLAYING) {
        DrawText("1:Split 2:Slow 3:Sweep", screenWidth/2 - 120, screenHeight - 30, 16, DARKGRAY);
    }

    if (!ball.IsLaunched() && currentState == GameState::MENU) {
        DrawText("Press SPACE to launch", screenWidth/2 - 120, screenHeight/2 + 100, 20, DARKGRAY);
    }

    switch (currentState) {
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
        default:
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
