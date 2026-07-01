#include "routes.h"

#include "api/v1/replay.h"

void api_make_routes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/v1/replay").methods(crow::HTTPMethod::Get)(api_replay_get);
    CROW_ROUTE(app, "/api/v1/replay").methods(crow::HTTPMethod::Post)(api_replay_post);
    CROW_ROUTE(app, "/api/v1/replay-data").methods(crow::HTTPMethod::Get)(api_replay_data_get);
}
