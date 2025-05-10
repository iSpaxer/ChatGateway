#include <uWebSockets/App.h>
#include <chrono>
#include <nlohmann/json.hpp>

#include "controllers/ChatController.h"
#include "util/logger/AsynsLogger.h"

using json = nlohmann::json;

AsyncLogger logger;

int main() {
    std::cout << "Запуск веб сервера 2..." << std::endl;
    auto uWS = uWS::App().get("/api/*", [](auto *res, auto */*req*/) {
        res->end("Hello world from Http!");
    }).listen(8080, [](const auto *listen_socket) {
        if (listen_socket) {
            logger << "Listening on port " << 8080 << std::endl;
        }
    });

    ChatController chatController(uWS);
    uWS.run();

    logger << "Failed to listen on port 8080" << std::endl;
    return 0;
}
