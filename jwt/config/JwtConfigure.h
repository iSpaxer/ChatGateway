//
// Created by alex on 5/11/25.
//

#ifndef JWTCONFIGURE_H
#define JWTCONFIGURE_H
#include <string>
#include <jwt-cpp/jwt.h>


class JwtConfigure {
public:
    explicit JwtConfigure();

    [[nodiscard]] std::string getSecret() const;

private:
    std::string base64UrlDecode(const std::string& input);

    std::string secret;
    const std::string keyB64url = "hi7S5RX5ZRZooHA0RKGctZ-KtR9FoESgCnH-3BNg5XI";
};



#endif //JWTCONFIGURE_H
