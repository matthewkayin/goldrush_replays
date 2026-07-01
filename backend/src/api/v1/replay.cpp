#include "replay.h"

#include "logger.h"
#include "api/util.h"
#include "database/sql.h"

crow::response api_replay_get(const crow::request& request) {
    Logger& logger = Logger::get_instance();
    try {
        const char* id_str = request.url_params.get("id");
        if (!id_str) {
            throw exceptionf("Query param 'id' is required.");
        }

        uint32_t id = atoi(id_str);

        SqlConnection connection;
        SqlStatement statement = connection.prepare("select name, data from match where id = ?");
        statement.bind_int(1, id);

        SqlResult result = statement.execute();
        if (result.empty()) {
            return api_response_error(404, (std::string("Match with ID") + id_str + " does not exist.").c_str());
        }
        if (result.size() > 1) {
            logger.warn("Multiple rows returned for match id %u. Row count: %u", id, result.size());
        }

        std::string& result_name = std::get<std::string>(result[0][0]);
        SqlBlob result_blob = std::get<SqlBlob>(result[0][1]);

        crow::response response;
        response.code = 200;
        response.set_header("Content-Type", "application/octet-stream");
        response.set_header("Content-Disposition", "attachment; filename=\"" + result_name + "\"");
        response.write(std::string(result_blob.begin(), result_blob.end()));

        return response;
    } catch (const std::exception& e) {
        logger.error("Exception in /api/v1/replay GET: %s", e.what());
        return api_response_error(400, e.what());
    }
}
