#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"
#include <vector>

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
    int score;
    int lives;
    int winCount;
    bool gameOver;
    bool paused;
    bool victory;
    float gameTime;
    
    // 砖块颜色
    Color brickColors[5];
    
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
