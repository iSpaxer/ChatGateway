#include <uWebSockets/App.h>
#include <librdkafka/rdkafkacpp.h>
#include <iostream>
#include <string>
#include <set>
#include <memory>

#include "controllers/ChatController.h"
#include "dto/ActiveUser.h"
#include "util/SimpleEnv.h"

int main(int argc, char* argv[]) {
    // Define WebSocket type and connections set

    SimpleEnv env(argc > 1 && std::string(argv[1]) == "prod" ? "/app/prod.env" : "dev.env");

    // Set up Kafka producer
    std::string errstr;
    auto host = env.get("KAFKA_HOST") + ":" + env.get("KAFKA_PORT");
    std::cout << "Хост порт: " << host << std::endl;
    std::unique_ptr<RdKafka::Conf> conf(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
    conf->set("bootstrap.servers", host, errstr);
    std::shared_ptr<RdKafka::Producer> producer(RdKafka::Producer::create(conf.get(), errstr));
    if (!producer) {
        std::cerr << "Failed to create Kafka producer: " << errstr << std::endl;
        return 1;
    }

    // Create Kafka topic
    std::shared_ptr<RdKafka::Topic> topic(RdKafka::Topic::create(producer.get(), "chat", nullptr, errstr));
    if (!topic) {
        std::cerr << "Failed to create topic: " << errstr << std::endl;
        return 1;
    }

    // ----------------
    //  Utils
    // ----------------
    JwtConfigure jwtConfig;

    // Set up uWebSockets server
    uWS::App app;

    // ----------------
    //  Controllers
    // ----------------
    ChatController chatController(app, jwtConfig, producer, topic);


    app.listen(3000, [](auto *listen_socket) {
        if (listen_socket) {
            std::cout << "Server running on port 3000" << std::endl;
        } else {
            std::cerr << "Failed to listen on port 3000" << std::endl;
        }
    });

    app.run();
    return 0;
}
