#include <uWebSockets/App.h>

#include <uWebSockets/App.h>
#include <amqpcpp.h>
#include <amqpcpp/libuv.h>
#include <uv.h>

#include "dto/ActiveUser.h"


int main() {
    // Объявляем очередь (на случай, если её ещё нет)
    //     // Объявляем очередьnt main() {
    // 1. Берём стандартный uv_loop_t
    uv_loop_t *uvLoop = uv_default_loop();

    // 2. Создаём us_loop_t поверх uv_loop_t
    //    Второй, третий и четвёртый параметры (wakeup_cb, pre_cb, post_cb) можно передать nullptr,
    //    ext_size оставляем 0, если не нужен дополнительный экстеншн.
    struct us_loop_t *usLoop = us_create_loop(
        /* hint */ (void *) uvLoop,
                   /* wakeup_cb */ nullptr,
                   /* pre_cb */ nullptr,
                   /* post_cb */ nullptr,
                   /* ext_size */ 0
    );

    // 3. Получаем C++ обёртку uWS::Loop
    uWS::Loop *uwsLoop = uWS::Loop::get(usLoop);

    // AMQP-CPP setup
    AMQP::LibUvHandler handler(uvLoop);
    AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://guest:guest@localhost/"));
    AMQP::TcpChannel channel(&connection);


    // uWS setup
    uWS::App()
            .ws<ActiveUser>("/*", {
                                .message = [&](auto *ws, std::string_view message, uWS::OpCode opCode) {
                                    channel.declareQueue("my_queue").onSuccess([](const std::string& name, uint32_t messageCount, uint32_t consumerCount) {
                                        std::cout << "Queue declared: " << name << "\n"
                                                << "Messages: " << messageCount << "\n"
                                                << "Consumers: " << consumerCount << std::endl;
                                    });
                                    channel.publish("", "my_queue", message);
                                    ws->send("broadcastMessage", opCode);
                                }
                            })
        .listen(9001, [](auto *token) {
            if (token) {
                std::cout << "Server is listening on port 9001\n";
            }
        });
    channel.publish("", "my_queue", "Hello, RabbitMQ!");

    uv_run(uvLoop, UV_RUN_DEFAULT);

}

// #include <chrono>
// #include <nlohmann/json.hpp>
//
// // #include "controllers/ChatController.h"
// #include "jwt/config/JwtConfigure.h"
// #include "util/logger/AsynsLogger.h"
//
// #include <amqpcpp.h>
// #include <amqpcpp/libuv.h>
// #include <uv.h>
//
// #include "dto/ActiveUser.h"
//
// using json = nlohmann::json;
//
// AsyncLogger logger;
//
// // #include <uWS/uWS.h>
// #include <amqpcpp.h>
// #include <amqpcpp/libuv.h>
// #include <iostream>
// #include <string>
//
// // Логгер (замените на ваш вариант)
// #define logger std::cout
//
// // Класс для обработки AMQP-событий
// class MyAMQPHandler : public AMQP::LibUvHandler {
// public:
//     MyAMQPHandler(uv_loop_t *loop) : AMQP::LibUvHandler(loop) {}
//     virtual void onData(AMQP::Connection *connection, const char *data, size_t size)  {
//         connection->parse(data, size);
//     }
//     virtual void onError(AMQP::Connection *connection, const char *message)  {
//         logger << "AMQP error: " << message << std::endl;
//     }
//     virtual void onConnected(AMQP::Connection *connection)  {
//         logger << "AMQP connected" << std::endl;
//     }
// };
//
// // Класс ChatController
// class ChatController {
// public:
//     ChatController(uWS::App &app, uv_loop_t *loop) : app_(app) {
//         // Настройка AMQP
//         amqp_handler_ = std::make_unique<MyAMQPHandler>(loop);
//         amqp_connection_ = std::make_unique<AMQP::TcpConnection>(amqp_handler_.get(), AMQP::Address("amqp://guest:guest@localhost/"));
//         amqp_channel_ = std::make_unique<AMQP::TcpChannel>(amqp_connection_.get());
//
//         // Объявляем обменник и очередь
//         // amqp_channel_->declareExchange("chat_exchange", AMQP::topic).onSuccess([]() {
//         //     logger << "Exchange chat_exchange declared" << std::endl;
//         // });
//         // amqp_channel_->declareQueue("chat_queue").onSuccess([]() {
//         //     logger << "Queue chat_queue declared" << std::endl;
//         // });
//         amqp_channel_->declareQueue("my_queue").onSuccess([](const std::string& name, uint32_t messageCount, uint32_t consumerCount) {
//         std::cout << "Queue declared: " << name << "\n"
//                   << "Messages: " << messageCount << "\n"
//                   << "Consumers: " << consumerCount << std::endl;
//         });
//         amqp_channel_->publish("", "my_queue", "Hello, RabbitMQ!");
//         // amqp_channel_->bindQueue("chat_exchange", "chat_queue", "chat.#").onSuccess([]() {
//         //     logger << "Queue bound to exchange" << std::endl;
//         // });
//
//         // Настройка WebSocket
//         app_.ws<ActiveUser>("/*", {
//             .message = [this](auto *ws, std::string_view message, uWS::OpCode opCode) {
//                 handleWebSocketMessage(ws, message, opCode);
//             }
//         });
//     }
//
//     void handleWebSocketMessage(auto *ws, std::string_view message, uWS::OpCode opCode) {
//         // Логируем полученное сообщение
//         logger << "Received WebSocket message: " << message << std::endl;
//
//         // Отправляем сообщение в RabbitMQ
//         std::string msg(message);
//         amqp_channel_->publish("", "my_queue", msg);
//         logger << "Sent to RabbitMQ: " << msg << std::endl;
//
//         // Можно отправить подтверждение клиенту через WebSocket
//         ws->send("Message received and sent to RabbitMQ", opCode);
//     }
//
// private:
//     uWS::App &app_;
//     std::unique_ptr<MyAMQPHandler> amqp_handler_;
//     std::unique_ptr<AMQP::TcpConnection> amqp_connection_;
//     std::unique_ptr<AMQP::TcpChannel> amqp_channel_;
// };
//
// // Основной код
// int main() {
//     logger << "Запуск ChatGateway сервера..." << std::endl;
//
//     // Инициализация цикла событий libuv
//     uv_loop_t *loop = uv_default_loop();
//
//     // Настройка uWebSockets
//     uWS::App app;
//
//     app.get("/api/*", [](auto *res, auto */*req*/) {
//         res->end("Hello world from Http!");
//     }).listen(8080, [](const auto *listen_socket) {
//         if (listen_socket) {
//             logger << "Listening on port 8080" << std::endl;
//         }
//     });
//
//     // Инициализация ChatController
//     ChatController chatController(app, loop);
//
//     // Запуск цикла событий
//     app.run();
//
//     return 0;
// }
