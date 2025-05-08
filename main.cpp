#include <iostream>
#include <uWebSockets/App.h>

int main()
{
    std::cout << "Запуск веб сервера..." << std::endl;
    uWS::App()
        .ws<int>("/*",
                 {// Используем int вместо void для UserData
                  .open =
                      [](auto *ws) {
                          std::cout << "New connection established" << std::endl;
                          ws->send("Hello, client!", uWS::OpCode::TEXT);
                      },
                  .message =
                      [](auto *ws, std::string_view message, uWS::OpCode opCode) {
                          std::cout << "Received: " << message << std::endl;
                          ws->send(message, opCode); // Эхо: отправляем сообщение обратно
                      },
                  .close =
                      [](auto *ws, int code, std::string_view message) {
                          std::cout << "Connection closed: " << message << std::endl;
                      }})
        .listen(9001,
                [](auto *listen_socket) {
                    if (listen_socket) {
                        std::cout << "Listening on port 9001" << std::endl;
                    } else {
                        std::cerr << "Failed to listen on port 9001" << std::endl;
                    }
                })
        .run();

    return 0;
}
