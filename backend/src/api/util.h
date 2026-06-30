#pragma once

#include <crow.h>

// Response
crow::response api_response_error(int code, const char* message);

// Query string
bool api_query_string_validate_uint(const char* param);
bool api_query_string_validate_uint_list(const char* param);
bool api_query_string_validate_date_string(const char* date);

// Date
std::string api_today_string();
