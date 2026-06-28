#include "match.h"

#include "logger.h"

crow::response api_match_post() {
    Logger& logger = Logger::get_instance();
    logger.info("Invoked /api/v1/match POST.");

    return crow::response(200, "{}");
}
