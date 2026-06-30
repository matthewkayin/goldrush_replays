#pragma once

#include <cstdint>
#include <string>

struct DateTime {
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;

    static DateTime now();
    static DateTime from_iso_string(const char* iso_string);
    std::string to_iso_string() const;
};
