//
// Created by alex on 5/10/25.
//

#ifndef CHATCONTROLLER_H
#define CHATCONTROLLER_H
#include <App.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ChatController {
public:
    explicit ChatController(uWS::App& _uWS);

    void createChat(uWS::HttpResponse<false> *res, uWS::HttpRequest *req);

private:
    std::string generateUUID();

    uWS::App& uWS;
    std::unordered_map<std::string, long> onlineByTopic;
    std::set<std::string> topics;
};



#endif //CHATCONTROLLER_H
