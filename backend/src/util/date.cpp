#include "date.h"

#include <ctime>

std::string date_util_today_string() {
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
