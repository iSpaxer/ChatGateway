//
// Created by alex on 5/10/25.
//

#ifndef CHATCONTROLLER_H
#define CHATCONTROLLER_H
#include <App.h>
#include <nlohmann/json.hpp>

#include "jwt/config/JwtConfigure.h"

using json = nlohmann::json;

class ChatController {
public:
    explicit ChatController(uWS::App& _uWS, JwtConfigure& _jwtConfigure);

private:
    uWS::App& uWS;
    JwtConfigure& jwtConfigure;
    std::unordered_map<std::string, long> onlineByTopic;
    std::set<std::string> topics;
};



#endif //CHATCONTROLLER_H
