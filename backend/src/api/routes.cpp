#include "routes.h"

#include "api/v1/match.h"

void api_make_routes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/v1/match")
        .methods(crow::HTTPMethod::Get)(api_match_get);
    CROW_ROUTE(app, "/api/v1/match")
        .methods(crow::HTTPMethod::Post)(api_match_post);
}
