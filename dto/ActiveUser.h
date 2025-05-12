//
// Created by alex on 5/10/25.
//

#ifndef ACTRIVEUSER_H
#define ACTRIVEUSER_H
#include <chrono>

#include "jwt/JwtToken.h"

// todo конструктор без аргументов?
class ActiveUser {
public:
    constexpr ActiveUser() = default;

    explicit ActiveUser(JwtToken jwtToken,  const std::string &ownerRoom)
        : jwtToken(std::move(jwtToken)), topic(ownerRoom), loggedIn(std::chrono::system_clock::now()) {
    }

    JwtToken jwtToken;
    std::string topic;
    std::chrono::system_clock::time_point loggedIn;
    std::chrono::system_clock::time_point loggedOut;
    std::chrono::system_clock::time_point lastActivity;
    long messageCount = 0;
};



#endif //ACTRIVEUSER_H
