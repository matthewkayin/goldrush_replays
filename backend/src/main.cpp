#define CROW_MAIN
#define ASIO_HAS_THREADS

#include "logger.h"
#include <crow.h>
#include <cstdio>

int main() {
    Logger logger(Logger::LogLevel::Debug);
    crow::logger::setHandler(&logger);
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]() {
        return "Hi friend";
    });

    try {
        logger.info("hello friend");
        app.bindaddr("127.0.0.1").port(3000).multithreaded().run();
    } catch (const std::exception& ex) {
        printf("Error: %s\n", ex.what());
    }

    return 0;
}
