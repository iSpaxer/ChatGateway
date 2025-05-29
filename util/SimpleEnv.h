//
// Created by alex on 5/29/25.
//

#ifndef SIMPLEENV_H
#define SIMPLEENV_H

#include <map>
#include <string>

class SimpleEnv {
public:
    explicit SimpleEnv(const std::string &filename);

private:
    std::map<std::string, std::string> vars;
public:
    bool load(const std::string& filename);

    std::string get(const std::string& key, const std::string& def="") const;
};



#endif //SIMPLEENV_H
