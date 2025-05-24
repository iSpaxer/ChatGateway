//
// Created by alex on 5/10/25.
//

#include "AsynsLogger.h"

#include <iomanip>

AsyncLogger::AsyncLogger() {
    // Запускаем поток логирования
    logThread = std::thread([this] {
        while (true) {
            std::string message;
            {
                std::unique_lock<std::mutex> lock(logQueueMutex);
                logCv.wait(lock, [this] { return !logQueue.empty() || !running; });
                if (!running && logQueue.empty()) break;
                message = std::move(logQueue.front());
                logQueue.pop();
            }
            // Получаем текущее системное время
            auto now = std::chrono::system_clock::now();

            // Преобразуем в time_t (время в секундах с эпохи)
            std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

            // Преобразуем в структуру tm в локальном времени
            std::tm* local_tm = std::localtime(&now_time_t);
            // Выводим сообщение
            std::cout << "[" << std::put_time(local_tm, "%Y-%m-%d %H:%M:%S")  << "] " << message << std::endl;
        }
    });
}

AsyncLogger::~AsyncLogger() {
    // Останавливаем поток
    {
        std::lock_guard<std::mutex> lock(logQueueMutex);
        running = false;
    }
    logCv.notify_one();
    logThread.join();
}

AsyncLogger & AsyncLogger::operator<<(std::ostream &(*manip)(std::ostream &)) {
    // Применяем манипулятор к буферу
    buffer << manip;
    // Проверяем, не пора ли отправить строку (например, после std::endl)
    if (buffer.str().find('\n') != std::string::npos) {
        std::lock_guard<std::mutex> lock(logQueueMutex);
        logQueue.push(buffer.str());
        buffer.str(""); // Очищаем буфер
        buffer.clear();
        logCv.notify_one();
    }
    return *this;
}
