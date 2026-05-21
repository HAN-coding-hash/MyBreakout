#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include <vector>
#include <string>

struct LevelData {
    std::string name;
    int rows;
    int cols;
    float brickWidth;
    float brickHeight;
    float spacingX;
    float spacingY;
    float startX;
    float startY;
    std::vector<std::vector<int>> bricks;
};

class LevelManager {
private:
    std::vector<LevelData> levels;
    int currentLevel;
    
public:
    LevelManager() : currentLevel(1) {
        // 第一关
        LevelData l1;
        l1.name = "第一关";
        l1.rows = 5; l1.cols = 8;
        l1.brickWidth = 85; l1.brickHeight = 25;
        l1.spacingX = 95; l1.spacingY = 35;
        l1.startX = 50; l1.startY = 80;
        l1.bricks = {
            {1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1},
            {1,1,1,1,1,1,1,1}
        };
        levels.push_back(l1);
        
        // 第二关
        LevelData l2;
        l2.name = "第二关";
        l2.rows = 5; l2.cols = 8;
        l2.brickWidth = 85; l2.brickHeight = 25;
        l2.spacingX = 95; l2.spacingY = 35;
        l2.startX = 50; l2.startY = 80;
        l2.bricks = {
            {1,0,1,0,1,0,1,0},
            {0,1,0,1,0,1,0,1},
            {1,0,1,0,1,0,1,0},
            {0,1,0,1,0,1,0,1},
            {1,0,1,0,1,0,1,0}
        };
        levels.push_back(l2);
        
        // 第三关
        LevelData l3;
        l3.name = "第三关";
        l3.rows = 5; l3.cols = 8;
        l3.brickWidth = 85; l3.brickHeight = 25;
        l3.spacingX = 95; l3.spacingY = 35;
        l3.startX = 50; l3.startY = 80;
        l3.bricks = {
            {1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1}
        };
        levels.push_back(l3);
    }
    
    bool LoadLevel(int levelNum) {
        if (levelNum >= 1 && levelNum <= (int)levels.size()) {
            currentLevel = levelNum;
            return true;
        }
        return false;
    }
    
    LevelData& GetCurrentLevel() { return levels[currentLevel - 1]; }
    int GetCurrentLevelNum() { return currentLevel; }
    bool HasNextLevel() { return currentLevel < (int)levels.size(); }
    void NextLevel() { if (HasNextLevel()) currentLevel++; }
    int GetTotalLevels() { return levels.size(); }
};

#endif
