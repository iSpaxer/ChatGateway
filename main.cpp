#include <uWebSockets/App.h>

// #include <uWebSockets/App.h>
// #include <amqpcpp.h>
// #include <amqpcpp/libuv.h>
// #include <uv.h>
//
// #include "dto/ActiveUser.h"
//
// #include <amqpcpp.h>
// #include <amqpcpp/libuv.h>
// #include <iostream>
// #include <memory>
//
// #include "dto/ActiveUser.h"
//
// // Логгер
// #include <amqpcpp.h>               // AMQP-CPP
// #include <amqpcpp/libuv.h>         // AMQP-CPP handler для libuv
// #include <uv.h>                    // libuv
// #include <iostream>
// #include <string>
//
// #include <uv.h>
// #include <amqpcpp.h>
// #include <amqpcpp/libuv.h>
// #include <iostream>
// #include <string>
//
//
// #include <nlohmann/json.hpp>
//
// // #include "controllers/ChatController.h"
// #include "jwt/config/JwtConfigure.h"
// #include "util/logger/AsynsLogger.h"
//
// #include <amqpcpp.h>
// #include <amqpcpp/libuv.h>
#include <uv.h>
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

// Структура для пользовательских данных WebSocket
struct PerSocketData {
    /* Пустая структура, можно добавить свои данные */
};

// Определяем структуру, соответствующую внутренней структуре us_loop_t
struct us_loop_t {
    char data[104];        // Буфер для выравнивания, размер зависит от версии uSockets
    uv_loop_t* uv_loop;    // Указатель на uv_loop_t
    int is_default;        // Флаг, указывающий, является ли цикл стандартным
    void* uv_pre;          // Дополнительные указатели для libuv
    void* uv_check;
};

int main() {
    // Создаем приложение uWebSockets
    uWS::App app;

    // Получаем указатель на цикл событий (uv_loop_t)
    uWS::Loop* loop = uWS::Loop::get();
    uv_loop_t* uv_loop = reinterpret_cast<us_loop_t *>(loop)->uv_loop;

    // Выводим адрес указателя для демонстрации
    std::cout << "uv_loop_t pointer: " << uv_loop << std::endl;

    // Настраиваем WebSocket-поведение
    app.ws<PerSocketData>("/*", {
        .open = [](auto* ws) {
            std::cout << "New WebSocket connection opened" << std::endl;
        },
        .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
            std::cout << "Received message: " << message << std::endl;
            ws->send(message, opCode); // Эхо-ответ
        },
        .close = [](auto* ws, int code, std::string_view message) {
            std::cout << "WebSocket closed: " << code << ", message: " << message << std::endl;
        }
    });

    // Запускаем приложение на порту 3000
    app.listen(3000, [](auto* listen_socket) {
        if (listen_socket) {
            std::cout << "Listening on port 3000" << std::endl;
        } else {
            std::cout << "Failed to listen on port 3000" << std::endl;
        }
    });

    // Запускаем цикл событий через run()
    app.run();

    std::cout << "Application has stopped" << std::endl;

    return 0;
}
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
//     app.getLoop();
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
