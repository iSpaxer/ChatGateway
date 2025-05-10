//
// Created by alex on 5/10/25.
//

#ifndef ACTRIVEUSER_H
#define ACTRIVEUSER_H
#include <chrono>

// todo конструктор без аргументов?
class ActiveUser {
public:
    constexpr ActiveUser() = default;
    explicit ActiveUser(const std::string &username, const std::string &ownerRoom)
        : username(username), topic(ownerRoom), loggedIn(std::chrono::system_clock::now()) {
    }

    std::string username;
    std::string topic;
    std::chrono::system_clock::time_point loggedIn;
    std::chrono::system_clock::time_point loggedOut;
    std::chrono::system_clock::time_point lastActivity;
    long messageCount = 0;
};



#endif //ACTRIVEUSER_H
