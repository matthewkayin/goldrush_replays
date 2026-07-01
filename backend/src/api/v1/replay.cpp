#include "replay.h"

#include "database/repository/replay.h"
#include "api/util.h"
#include "logger.h"

crow::response api_replay_get(const crow::request& request) {
    Logger& logger = Logger::get_instance();
    logger.info("Invoked /api/v1/replay GET.");

    try {
        // Build query params
        ReplayGetQueryParams query_params;
        api_get_param(request, "ids", &query_params.ids);
        api_get_param(request, "date_from", &query_params.date_from);
        api_get_param(request, "date_to", &query_params.date_to);
        api_get_param(request, "name_contains", &query_params.name_contains);

        // Pagination query params
        const char* offset_str = request.url_params.get("offset");
        uint32_t offset = offset_str ? atoi(offset_str) : 0;
        const char* limit_str = request.url_params.get("limit");
        uint32_t limit = limit_str ? atoi(limit_str) : 10;

        // Get records
        uint32_t record_count = replay_repository_get_count(query_params);
        std::vector<ReplayGetRecord> records = replay_repository_get(query_params, offset, limit);
        uint32_t records_remaining = (uint32_t)std::max((int64_t)record_count - (int64_t)(offset + limit), (int64_t)0);

        // Convert response records to JSON
        std::vector<crow::json::wvalue> response_data;
        for (const ReplayGetRecord& record : records) {
            response_data.push_back(record.to_json());
        }

        crow::json::wvalue response;
        response["data"] = std::move(response_data);
        response["remaining"] = records_remaining;

        return response;
    } catch (const std::exception& e) {
        logger.error("Exception in /api/v1/replay GET: %s", e.what());
        return api_response_error(400, e.what());
    }
}

crow::response api_replay_post(const crow::request& request) {
    Logger& logger = Logger::get_instance();
    logger.info("Invoked /api/v1/replay POST.");

    try {
        // Build post records from multipart body
        std::vector<ReplayPostRecord> post_records;
        crow::multipart::message message(request);
        for (const auto& part : message.parts) {
            auto content_disposition = part.headers.find("Content-Disposition");
            std::string name = content_disposition->second.params.find("name")->second;
            auto filename_it = content_disposition->second.params.find("filename");
            bool is_file = filename_it != content_disposition->second.params.end();
            if (!is_file) {
                throw exceptionf("Invalid part %s.", name.c_str());
            }

            std::string filename = filename_it->second;
            std::string file_content = part.body;

            post_records.push_back({
                .name = filename.c_str(),
                .date = api_today_string(),
                .duration = "0:00",
                .data = part.body
            });
        }

        // Post
        std::vector<uint32_t> ids = replay_repository_post(post_records);

        // Build query params
        ReplayGetQueryParams query_params;
        for (uint32_t id_index = 0; id_index < ids.size(); id_index++) {
            if (id_index != 0) {
                query_params.ids += ",";
            }
            query_params.ids += std::to_string(ids[id_index]);
        }

        // Get response records
        std::vector<ReplayGetRecord> get_records = replay_repository_get(query_params, REPLAY_GET_OFFSET_NONE, REPLAY_GET_LIMIT_NONE);

        // Convert response records into JSON
        std::vector<crow::json::wvalue> get_record_jsons;
        for (const ReplayGetRecord& record : get_records) {
            get_record_jsons.push_back(record.to_json());
        }

        crow::json::wvalue response;
        response["data"] = std::move(get_record_jsons);

        return response;
    } catch (const std::exception& e) {
        logger.error("Exception in /api/v1/replay POST: %s", e.what());
        return api_response_error(400, e.what());
    }
}

crow::response api_replay_data_get(const crow::request& request) {
    Logger& logger = Logger::get_instance();
    logger.info("Invoked /api/v1/replay-data GET.");

    try {
        const char* id_str = request.url_params.get("id");
        if (!id_str) {
            throw exceptionf("Query param 'id' is required.");
        }

        ReplayDataRecord record = replay_repository_get_data((uint32_t)atoi(id_str));

        crow::response response;
        response.code = 200;
        response.set_header("Content-Type", "application/octet-stream");
        response.set_header("Content-Disposition", "attachment; filename=\"" + record.name + "\"");
        response.write(std::string(record.data.begin(), record.data.end()));

        return response;
    } catch (const std::exception& e) {
        logger.error("Exception in /api/v1/replay-data GET: %s", e.what());
        return api_response_error(400, e.what());
    }
}
