//
// Created by alex on 5/10/25.
//

#include "dto/ActiveUser.h"
#include "ChatController.h"

ChatController::ChatController(uWS::App &_uWS): uWS(_uWS) {
    uWS.post("/api/create/chat", [this](auto *res, auto *req) {
        createChat(res, req);
    })
    .ws<ActiveUser>("/ws/*", {
        .upgrade = [this](uWS::HttpResponse<false> *res, uWS::HttpRequest *req, struct us_socket_context_t *context) {
            std::string url = std::string(req->getUrl());
            if (url.length() > 4) {
                std::string topic =  url.substr( 4);
                std::string username = generateUUID(); // todo достать username из JWT
                if (!topic.empty() && topics.contains(topic)) {
                    ActiveUser activeUser(username, topic);
                    res->upgrade<ActiveUser>(std::move(activeUser), req->getHeader("sec-websocket-key"),
                                            req->getHeader("sec-websocket-protocol"),
                                            req->getHeader("sec-websocket-extensions"),
                                            context);
                    // todo отправка в кафку. Что к нам хочет соединение новое
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
            std::string broadcastMessage = std::string(activeUser->username) + ": " + std::string(message);
            ws->publish(activeUser->topic, broadcastMessage, opCode);
        },
        .close = [this](uWS::WebSocket<false, true, ActiveUser> *ws, int code, std::string_view message) {
                    ActiveUser *activeUser = ws->getUserData();
                    // Оповещаем топик об уходе
                    ws->publish(activeUser->topic, activeUser->username + " left the topic.", uWS::OpCode::TEXT);

                    onlineByTopic[activeUser->topic]--;
                    // Освобождаем ClientData
                    delete activeUser;
                }
    });


}

void ChatController::createChat(uWS::HttpResponse<false> *res, uWS::HttpRequest *req) {
    std::string body;

    res->onAborted([res]() {
        std::cout << "Request aborted" << std::endl;
        // Не нужно вызывать res->end(), так как соединение уже прервано
    });

    res->onData([res, body = std::move(body), this](const std::string_view data, const bool last) mutable {
        body.append(data);
        // Если это последняя часть данных
        if (last) {
            try {
                // Парсим JSON
                auto user = json::parse(body);

                // Проверяем наличие полей
                if (!user.contains("username")) {
                    res->writeStatus("400 Bad Request")
                            ->writeHeader("Content-Type", "application/json")
                            ->end(json{{"error", "Missing username"}}.dump());
                    return;
                }

                // Извлекаем данные
                const std::string username = user["username"];

                // Пример валидации
                if (username.empty()) {
                    res->writeStatus("400 Bad Request")
                            ->writeHeader("Content-Type", "application/json")
                            ->end(json{{"error", "Invalid username"}}.dump());
                    return;
                }

                topics.insert(username);

                res->writeStatus("201 Created")
                        ->writeHeader("Content-Type", "application/json")
                        ->end("Chat was launched successfully!");
            } catch (const json::parse_error& e) {
                // Ошибка парсинга JSON
                res->writeStatus("400 Bad Request")
                        ->writeHeader("Content-Type", "application/json")
                        ->end(json{{"error", "Invalid JSON format"}}.dump());
            } catch (const std::exception& e) {
                // Другие ошибки
                res->writeStatus("500 Internal Server Error")
                        ->writeHeader("Content-Type", "application/json")
                        ->end(json{{"error", "Server error"}}.dump());
            }
        }
    });
}

std::string ChatController::generateUUID() {
    std::stringstream ss;

    // Инициализируем генератор случайных чисел
    std::srand(std::time(nullptr));

    for (int i = 0; i < 16; ++i) {
        int byte = std::rand() % 256;

        // Устанавливаем версию (4) и variant (8, 9, A или B)
        if (i == 6) {
            byte = (byte & 0x0F) | 0x40; // Версия 4
        } else if (i == 8) {
            byte = (byte & 0x3F) | 0x80; // Variant 10xxxxxx
        }

        ss << std::hex << std::setw(2) << std::setfill('0') << byte;

        // Вставляем дефисы
        if (i == 3 || i == 5 || i == 7 || i == 9)
            ss << "-";
    }

    return ss.str();
}
