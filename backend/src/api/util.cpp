#include "util.h"

#include <ctime>

// RESPONSE

crow::response api_response_error(int code, const char* message) {
    crow::json::wvalue response_json;
    response_json["message"] = message;
    return crow::response(code, response_json.dump());
}

// URL PARAMS

void api_get_param(const crow::request& request, const char* name, std::string* out_str) {
    const char* param = request.url_params.get(name);
    if (param) {
        *out_str = std::string(param);
    }
}

// QUERY STRING

bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool api_query_string_validate_uint(const char* param) {
    const char* param_ptr = param;
    while (*param_ptr != '\0') {
        if (!is_digit(*param_ptr)) {
            return false;
        }
        param_ptr++;
    }

    return true;
}

bool api_query_string_validate_uint_list(const char* param) {
    const char* param_ptr = param;
    while (*param_ptr != '\0') {
        if (!(is_digit(*param_ptr) || *param_ptr == ',')) {
            return false;
        }
        param_ptr++;
    }

    return true;
}

bool api_query_string_validate_date_string(const char* date_str) {
    uint32_t year, month, day;
    int items_parsed = sscanf(date_str, "%d-%d-%d",
        &year, &month, &day);
    return items_parsed == 3 && strlen(date_str) == strlen("YYYY-MM-DD");
}

// DATE

std::string api_today_string() {
    time_t now;
    tm* utc_time;
    time(&now);
    utc_time = gmtime(&now);

    uint32_t year = 1900 + utc_time->tm_year;
    uint32_t month = 1 + utc_time->tm_mon;
    uint32_t day = utc_time->tm_mday;

    char buffer[16];
    sprintf(buffer, "%04d-%02d-%02d", year, month, day);
    return std::string(buffer);
}
