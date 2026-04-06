#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

class Config {
private:
    json data;
    
public:
    Config(const std::string& filename) {
        std::ifstream file(filename);
        if (file.is_open()) {
            file >> data;
            std::cout << "配置文件加载成功: " << filename << std::endl;
        } else {
            std::cerr << "无法打开配置文件: " << filename << std::endl;
        }
    }
    
    int getInt(const std::string& section, const std::string& key) {
        return data[section].value(key, 0);
    }
    
    float getFloat(const std::string& section, const std::string& key) {
        return data[section].value(key, 0.0f);
    }
    
    std::string getString(const std::string& section, const std::string& key) {
        return data[section].value(key, "");
    }
};

#endif
