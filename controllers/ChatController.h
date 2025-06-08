//
// Created by alex on 5/10/25.
//

#ifndef CHATCONTROLLER_H
#define CHATCONTROLLER_H
#include <uWebSockets/App.h>
#include <nlohmann/json.hpp>
#include <librdkafka/rdkafkacpp.h>

#include "jwt/config/JwtConfigure.h"

using json = nlohmann::json;

class ChatController {
public:
    explicit ChatController(uWS::App& _uWS,
        JwtConfigure& _jwtConfigure,
        std::shared_ptr<RdKafka::Producer> _producer,
        std::shared_ptr<RdKafka::Topic> _topic);

private:
    uWS::App& uWS;
    JwtConfigure& jwtConfigure;
    std::unordered_map<std::string, long> onlineByTopic;
    std::set<std::string> topics;
    std::shared_ptr<RdKafka::Producer> producer;
    std::shared_ptr<RdKafka::Topic> topic;
};



#endif //CHATCONTROLLER_H
