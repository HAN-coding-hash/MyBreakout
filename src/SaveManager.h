#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

class SaveManager {
private:
    json data;
    std::string filename;
    
public:
    SaveManager(const std::string& file = "save.json") : filename(file) {
        std::ifstream f(file);
        if (f.is_open()) {
            f >> data;
        } else {
            data["highScore"] = 0;
            data["currentLevel"] = 1;
        }
    }
    
    void Save() {
        std::ofstream f(filename);
        if (f.is_open()) f << data.dump(4);
    }
    
    int GetHighScore() { return data["highScore"]; }
    int GetCurrentLevel() { return data["currentLevel"]; }
    void SetHighScore(int score) { 
        if (score > data["highScore"]) {
            data["highScore"] = score; 
            Save();
        }
    }
    void SetCurrentLevel(int level) { 
        data["currentLevel"] = level; 
        Save();
    }
};

#endif
