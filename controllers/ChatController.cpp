//
// Created by alex on 5/10/25.
//

#include "dto/ActiveUser.h"
#include "ChatController.h"
#include <jwt-cpp/jwt.h>

ChatController::ChatController(uWS::App &_uWS, JwtConfigure& _jwtConfigure): uWS(_uWS), jwtConfigure(_jwtConfigure) {
    // -------------
    // Create chat
    // -------------
    uWS.post("/api/create/chat", [this](auto *res, auto *req) {
        std::string body;
        res->onAborted([res]() {
            std::cout << "Request aborted" << std::endl;
            // Не нужно вызывать res->end(), так как соединение уже прервано
        });
        auto jwtTokenStr = std::string(req->getHeader("bearer"));
        if (jwtTokenStr.empty()) {
            res->writeStatus("403")
                ->end("Invalid Access Token.");
            return;
        }
        JwtToken jwtToken;
        try {
            auto jwtTokenDecode = jwt::decode(jwtTokenStr);
            auto verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256(jwtConfigure.getSecret()));
            // verifier.verify(jwtToken); todo верификация отключена
            jwtToken = JwtToken(jwtTokenDecode);
        }
        catch (...) {
            res->writeStatus("403 Invalid Access Token")
                ->end("Invalid Access Token.");
            return;
        }

        res->onData([res, body = std::move(body), jwtToken, this](const std::string_view data, const bool last) mutable {
            body.append(data);
            if (last) {
                try {
                    topics.insert(jwtToken.username);

                    res->writeStatus("201 Created")
                            ->writeHeader("Content-Type", "application/json")
                            ->end("Chat was launched successfully!");
                } catch (const json::parse_error& e) {
                    res->writeStatus("400 Bad Request")
                            ->writeHeader("Content-Type", "application/json")
                            ->end(json{{"error", "Invalid JSON format"}}.dump());
                } catch (const std::exception& e) {
                    res->writeStatus("500 Internal Server Error")
                            ->writeHeader("Content-Type", "application/json")
                            ->end(json{{"error", "Server error"}}.dump());
                }
            }
        });
    })
    // -------------
    // Connecting in chat by Ws
    // -------------
    .ws<ActiveUser>("/ws/*", {
        .upgrade = [this](uWS::HttpResponse<false> *res, uWS::HttpRequest *req, struct us_socket_context_t *context) {
            if (auto url = std::string(req->getUrl()); url.length() > 4) {
                std::string topic =  url.substr( 4);

                auto jwtTokenStr = std::string(req->getHeader("bearer"));
                if (jwtTokenStr.empty()) {
                    res->writeStatus("403")
                        ->end("Invalid Access Token.");
                    return;
                }
                JwtToken jwtToken;
                try {
                    auto jwtTokenDecode = jwt::decode(jwtTokenStr);
                    auto verifier = jwt::verify()
                        .allow_algorithm(jwt::algorithm::hs256(jwtConfigure.getSecret()));
                    // verifier.verify(jwtToken); todo верификация отключена
                    jwtToken = JwtToken(jwtTokenDecode);
                }
                catch (...) {
                    res->writeStatus("403 Invalid Access Token")
                        ->end("Invalid Access Token.");
                    return;
                }

                if (!topic.empty() && topics.contains(topic)) {
                    ActiveUser activeUser(jwtToken, topic);
                    res->upgrade<ActiveUser>(std::move(activeUser), req->getHeader("sec-websocket-key"),
                                            req->getHeader("sec-websocket-protocol"),
                                            req->getHeader("sec-websocket-extensions"),
                                            context);
                    return;
                }
            }
            res->writeStatus("400 Bad Request")
                   ->end("Invalid WebSocket path.");
        },
        .open = [this](uWS::WebSocket<false, true, ActiveUser> *ws) {
            const auto* activeUser = ws->getUserData();
            ws->subscribe(activeUser->topic);
            onlineByTopic[activeUser->topic]++;
        },
        .message = [this](auto *ws, std::string_view message, uWS::OpCode opCode) {
            ActiveUser *activeUser = ws->getUserData();
            std::string broadcastMessage = std::string(activeUser->jwtToken.username) + ": " + std::string(message);
            ws->publish(activeUser->topic, broadcastMessage, opCode);
        },
        .close = [this](uWS::WebSocket<false, true, ActiveUser> *ws, int code, std::string_view message) {
                    ActiveUser *activeUser = ws->getUserData();
                    // Оповещаем топик об уходе
                    ws->publish(activeUser->topic, activeUser->jwtToken.username + " left the topic.", uWS::OpCode::TEXT);

                    onlineByTopic[activeUser->topic]--;
                }
    });
}