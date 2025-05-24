#include <jwt-cpp/jwt.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <iostream>
#include <string>
#include <memory>

// Функция для декодирования base64url с помощью OpenSSL
std::string base64url_decode(const std::string& input) {
    // Добавляем padding, если нужно
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

int main() {
    // JWT токен
    std::string token = "eyJhbGciOiJIUzI1NiIsImN1c3RvbSI6InZhbHVlIn0.eyJzdWIiOiJhbGV4YW5kciIsImV4cCI6MTc0Njk2NDA2OSwiaWF0IjoxNzQ2OTYzNzY5LCJhdXRob3JpdGllcyI6W119.XRbopv28QlJ6WJCuxB8Oqvj87Mlf-uZafFR6Pus-aVY";

    // Base64url-закодированный ключ из JWK
    std::string encoded_key = "hi7S5RX5ZRZooHA0RKGctZ-KtR9FoESgCnH-3BNg5XI";

    try {
        // Декодируем ключ с помощью OpenSSL
        std::string secret = base64url_decode(encoded_key);
        std::cout << "Ключ успешно декодирован, длина: " << secret.length() << " байт\n";

        // Декодируем JWT
        auto decoded = jwt::decode(token);

        // Верификатор для HS256
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256(secret));

        // Проверяем подпись
        verifier.verify(decoded);

        // Выводим payload
        std::cout << "JWT успешно верифицирован. Payload:\n";
        for (const auto& claim : decoded.get_payload_claims()) {
            std::cout << claim.first << ": " << claim.second.to_json() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}