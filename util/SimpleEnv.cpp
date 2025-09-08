//
// Created by alex on 5/29/25.
//

#include "SimpleEnv.h"

#include <fstream>

SimpleEnv::SimpleEnv(const std::string &filename) {
    load(filename);
}

bool SimpleEnv::load(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    std::string line;
    while (std::getline(file, line)) {
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        vars[key] = value;
        setenv(key.c_str(), value.c_str(), 1);
    }
    return true;
}

std::string SimpleEnv::get(const std::string &key, const std::string &def) const {
    const auto it = vars.find(key);
    return it != vars.end() ? it->second : def;
}
