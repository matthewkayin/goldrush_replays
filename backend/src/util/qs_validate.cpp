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
    uint32_t year, month, day;
    int items_parsed = sscanf(date_str, "%d-%d-%d",
        &year, &month, &day);
    return items_parsed == 3 && strlen(date_str) == strlen("YYYY-MM-DD");
}
