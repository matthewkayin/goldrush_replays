#include "match.h"

#include "database/repository/match.h"
#include "api/util.h"
#include "logger.h"

void api_match_get_param(const crow::request& request, const char* name, std::string* out_str) {
    const char* param = request.url_params.get(name);
    if (param) {
        *out_str = std::string(param);
    }
}

crow::response api_match_get(const crow::request& request) {
    Logger& logger = Logger::get_instance();
    logger.info("Invoked /api/v1/match GET.");

    try {
        // Build query params
        MatchGetQueryParams query_params;
        api_match_get_param(request, "ids", &query_params.ids);
        api_match_get_param(request, "date_from", &query_params.date_from);
        api_match_get_param(request, "date_to", &query_params.date_to);
        api_match_get_param(request, "name_contains", &query_params.name_contains);

        // Pagination query params
        const char* offset_str = request.url_params.get("offset");
        uint32_t offset = offset_str ? atoi(offset_str) : 0;
        const char* limit_str = request.url_params.get("limit");
        uint32_t limit = limit_str ? atoi(limit_str) : 10;

        // Get records
        uint32_t record_count = match_repository_get_count(query_params);
        std::vector<MatchGetRecord> records = match_repository_get(query_params, offset, limit);
        uint32_t records_remaining = (uint32_t)std::max((int64_t)record_count - (int64_t)(offset + limit), (int64_t)0);

        // Convert response records to JSON
        std::vector<crow::json::wvalue> response_data;
        for (const MatchGetRecord& record : records) {
            response_data.push_back(record.to_json());
        }

        crow::json::wvalue response;
        response["data"] = std::move(response_data);
        response["remaining"] = records_remaining;

        return response;
    } catch (const std::exception& e) {
        logger.error("Exception in /api/v1/match GET: %s", e.what());
        return api_response_error(400, e.what());
    }
}

crow::response api_match_post(const crow::request& request) {
    Logger& logger = Logger::get_instance();
    logger.info("Invoked /api/v1/match POST.");

    try {
        // Build post records from multipart body
        std::vector<MatchPostRecord> post_records;
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
        std::vector<uint32_t> ids = match_repository_post(post_records);

        // Build query params
        MatchGetQueryParams query_params;
        for (uint32_t id_index = 0; id_index < ids.size(); id_index++) {
            if (id_index != 0) {
                query_params.ids += ",";
            }
            query_params.ids += std::to_string(ids[id_index]);
        }

        // Get response records
        std::vector<MatchGetRecord> get_records = match_repository_get(query_params, MATCH_GET_OFFSET_NONE, MATCH_GET_LIMIT_NONE);

        // Convert response records into JSON
        std::vector<crow::json::wvalue> get_record_jsons;
        for (const MatchGetRecord& record : get_records) {
            get_record_jsons.push_back(record.to_json());
        }

        crow::json::wvalue response;
        response["data"] = std::move(get_record_jsons);

        return response;
    } catch (const std::exception& e) {
        logger.error("Exception in /api/v1/match POST: %s", e.what());
        return api_response_error(400, e.what());
    }
}
