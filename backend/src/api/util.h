#pragma once

#include <crow.h>

// Response
crow::response api_response_error(int code, const char* message);

// URL params
void api_get_param(const crow::request& request, const char* name, std::string* out_str);

// Query string
bool api_query_string_validate_uint(const char* param);
bool api_query_string_validate_uint_list(const char* param);
bool api_query_string_validate_date_string(const char* date);

// Date
std::string api_today_string();
