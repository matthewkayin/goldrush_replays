#define CROW_MAIN
#define ASIO_HAS_THREADS

#include "logger.h"
#include "api/routes.h"
#include <crow.h>
#include <cstdio>

int main() {
    Logger& logger = Logger::get_instance();
    crow::logger::setHandler(&logger);

    crow::SimpleApp app;
    api_make_routes(app);

    try {
        app.bindaddr("127.0.0.1").port(3000).multithreaded().run();
    } catch (const std::exception& ex) {
        printf("Error: %s\n", ex.what());
    }

    return 0;
}
