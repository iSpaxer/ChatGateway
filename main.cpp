#include <uWebSockets/App.h>
#include <librdkafka/rdkafkacpp.h>
#include <iostream>
#include <string>
#include <set>
#include <memory>

#include "dto/ActiveUser.h"

int main() {
    // Define WebSocket type and connections set

    // Set up Kafka producer
    std::string errstr;
    std::unique_ptr<RdKafka::Conf> conf(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
    conf->set("bootstrap.servers", "localhost:9092", errstr);
    std::unique_ptr<RdKafka::Producer> producer(RdKafka::Producer::create(conf.get(), errstr));
    if (!producer) {
        std::cerr << "Failed to create Kafka producer: " << errstr << std::endl;
        return 1;
    }

    // Create Kafka topic
    std::unique_ptr<RdKafka::Topic> topic(RdKafka::Topic::create(producer.get(), "chat", nullptr, errstr));
    if (!topic) {
        std::cerr << "Failed to create topic: " << errstr << std::endl;
        return 1;
    }

    // Set up uWebSockets server
    uWS::App app;

    app.ws<ActiveUser>("/*", {
        .open = [](uWS::WebSocket<false, true, ActiveUser> *ws) {
            const auto* activeUser = ws->getUserData();
            ws->subscribe(activeUser->topic);
        },
        .message = [&producer, &topic](auto *ws, std::string_view message, uWS::OpCode opCode) {
            std::string msg(message);

            // Send message to Kafka
            RdKafka::ErrorCode err = producer->produce(
                topic.get(),
                RdKafka::Topic::PARTITION_UA,
                RdKafka::Producer::RK_MSG_COPY,
                const_cast<char *>(msg.c_str()), msg.length(),
                nullptr, 0,
                nullptr
            );

            if (err != RdKafka::ERR_NO_ERROR) {
                std::cerr << "Failed to produce to Kafka: " << RdKafka::err2str(err) << std::endl;
            } else {
                std::cout << "Message sent to Kafka: " << msg << std::endl;
            }

            // Broadcast to all clients

            producer->poll(0);
        },
    });

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
