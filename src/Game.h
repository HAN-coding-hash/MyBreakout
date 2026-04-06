#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include "Config.h"
#include <vector>

// 游戏状态枚举
enum class GameState {
    MENU,       // 菜单/等待发射
    PLAYING,    // 游戏中
    PAUSED,     // 暂停
    GAME_OVER,  // 游戏结束（失败）
    VICTORY     // 胜利
};

class Game {
private:
    // 窗口
    int screenWidth;
    int screenHeight;
    
    // 游戏对象
    Ball ball;
    Paddle paddle;
    std::vector<Brick> bricks;
    
    // 游戏状态
    GameState currentState;  // 替换原来的 gameOver, paused, victory
    int score;
    int lives;
    int winCount;
    float gameTime;
    
    // 私有方法
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
