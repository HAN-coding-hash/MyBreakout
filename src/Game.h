#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include <vector>

// 游戏状态枚举
enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER,
    VICTORY
};

// 模式枚举
enum class PowerUpMode {
    NONE,
    SPLIT,
    SLOW_TIME,
    SWEEP
};

class Game {
private:
    int screenWidth;
    int screenHeight;
    
    Ball ball;
    Paddle paddle;
    std::vector<Brick> bricks;
    
    // 模式相关成员
    PowerUpMode currentMode;
    float modeTimer;
    std::vector<Ball> extraBalls;
    float originalBallSpeed;
    float originalBallRadius;
    float originalPaddleWidth;
    
    GameState currentState;
    int score;
    int lives;
    int winCount;
    float gameTime;
    
    void InitBricks();
    void RandomGoldenBrick();
    void HandleInput();
    void Update();
    void Draw();
    void CheckCollisions();
    void CheckGameOver();
    
public:
    Game();
    ~Game();
    void Run();
};

#endif
