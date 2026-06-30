#include "qs_validate.h"

#include "util/date.h"

bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool query_string_validate_uint(const char* param) {
    const char* param_ptr = param;
    while (*param_ptr != '\0') {
        if (!is_digit(*param_ptr)) {
            return false;
        }
        param_ptr++;
    }

    return true;
}

bool query_string_validate_uint_list(const char* param) {
    const char* param_ptr = param;
    while (*param_ptr != '\0') {
        if (!(is_digit(*param_ptr) || *param_ptr == ',')) {
            return false;
        }
        param_ptr++;
    }

    return true;
}

bool query_string_validate_date_iso_string(const char* date_str) {
    try {
        DateTime date = DateTime::from_iso_string(date_str);
        std::string iso_string = date.to_iso_string();
        return std::string(date_str) == iso_string;
    } catch (const std::exception&) {
        return false;
    }
}
