#include "match.h"

#include "logger.h"

crow::response api_match_post(const crow::request& request) {
    Logger& logger = Logger::get_instance();
    logger.info("Invoked /api/v1/match POST.");

    try {
        crow::multipart::message message(request);
        for (const auto& part : message.parts) {
            auto content_disposition = part.headers.find("Content-Disposition");
            std::string name = content_disposition->second.params.find("name")->second;
            auto filename_it = content_disposition->second.params.find("filename");
            bool is_file = filename_it != content_disposition->second.params.end();

            logger.info("Message part %s is_file ? %i", name.c_str(), (int)is_file);
            if (is_file) {
                std::string filename = filename_it->second;
                std::string file_content = part.body;

                FILE* file = fopen(filename.c_str(), "wb");
                if (!file) {
                    logger.error("Failed to open file with filename %s.", filename.c_str());
                    continue;
                }

                fwrite(file_content.data(), 1, file_content.length(), file);
                fclose(file);
                logger.info("Wrote %s.", filename.c_str());
            }
        }
    } catch (const std::exception& e) {
        logger.error("Exception in /api/v1/match POST: %s", e.what());
        return crow::response(400, e.what());
    }

    return crow::response(200, "{}");
}
