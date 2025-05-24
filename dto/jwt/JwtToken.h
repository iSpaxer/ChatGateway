//
// Created by alex on 5/12/25.
//

#ifndef JWTTOKEN_H
#define JWTTOKEN_H
#include <chrono>
#include <string>
#include <vector>

#include "jwt-cpp/jwt.h"

class JwtToken {

public:
    explicit JwtToken() = default;
    explicit JwtToken(const jwt::decoded_jwt<jwt::traits::kazuho_picojson> &jwtToken):
    username(jwtToken.get_payload_claims().at("sub").as_string()),
    createdAt(std::chrono::sys_seconds{std::chrono::seconds
        {jwtToken.get_payload_claims().at("iat").as_int()}
    }), expiresAt(std::chrono::sys_seconds{std::chrono::seconds
        {jwtToken.get_payload_claims().at("exp").as_int()}
    }) {

        auto authorities_claim = jwtToken.get_payload_claims().at("authorities").as_array();
        for (auto& auth : authorities_claim) {
            // authorities.push_back(auth.as_string());
        }
    }

    std::string username;
    std::vector<std::string> authorities;
    std::chrono::sys_seconds createdAt;
    std::chrono::sys_seconds expiresAt;
};
#endif //JWTTOKEN_H
