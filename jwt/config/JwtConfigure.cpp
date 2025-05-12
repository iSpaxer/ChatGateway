//
// Created by alex on 5/11/25.
//

#include "JwtConfigure.h"

std::string JwtConfigure::base64UrlDecode(const std::string &input) {
    std::string padded_input = input;
    if (padded_input.length() % 4 != 0) {
        padded_input += std::string(4 - padded_input.length() % 4, '=');
    }

    // Заменяем base64url символы на base64
    std::string base64_input = padded_input;
    std::replace(base64_input.begin(), base64_input.end(), '-', '+');
    std::replace(base64_input.begin(), base64_input.end(), '_', '/');

    // Декодируем с помощью OpenSSL
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bio = BIO_new_mem_buf(base64_input.data(), base64_input.length());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    std::vector<char> buffer((base64_input.length() * 3) / 4 + 1);
    int decoded_length = BIO_read(bio, buffer.data(), buffer.size());
    BIO_free_all(bio);

    if (decoded_length < 0) {
        throw std::runtime_error("Failed to decode base64url string");
    }

    return std::string(buffer.data(), decoded_length);
}
