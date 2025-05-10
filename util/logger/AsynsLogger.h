#ifndef ASYNC_LOGGER_H
#define ASYNC_LOGGER_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <sstream>
#include <string>

class AsyncLogger {
    std::queue<std::string> logQueue;
    std::mutex logQueueMutex;
    std::condition_variable logCv;
    std::thread logThread;
    bool running = true;
    std::ostringstream buffer; // Буфер для потоковой записи

public:
    AsyncLogger();

    ~AsyncLogger();

    // Шаблонный оператор << для любых типов
    template<typename T>
    AsyncLogger& operator<<(const T& value);

    // Оператор << для манипуляторов
    AsyncLogger& operator<<(std::ostream& (*manip)(std::ostream&));
};

template<typename T>
AsyncLogger & AsyncLogger::operator<<(const T &value) {
    buffer << value;
    return *this;
}

#endif // ASYNC_LOGGER_H
