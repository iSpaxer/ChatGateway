#include <uWebSockets/App.h>
#include <iostream>

// Пустая структура, если вам не нужно хранить per‑socket данные
struct PerSocketData {};

int main() {
    uWS::App().ws<PerSocketData>("/*", {
        .open = [](auto *ws) {
            std::cout << "Клиент подключился!" << std::endl;
        },
        .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
            std::cout << "Получено сообщение: " << message << std::endl;
            ws->send(message, opCode);
        },
        .close = [](auto *ws, int code, std::string_view message) {
            std::cout << "Клиент отключился!" << std::endl;
        }
    }).listen(9001, [](auto *token) {
        if (token) {
            std::cout << "Сервер слушает порт 9001" << std::endl;
        } else {
            std::cerr << "Ошибка запуска сервера!" << std::endl;
        }
    }).run();

    return 0;
}
