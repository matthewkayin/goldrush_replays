#include "date.h"

#include "logger.h"
#include <ctime>

DateTime DateTime::now() {
    DateTime result;
    time_t now;
    tm* utc_time;
    time(&now);
    utc_time = gmtime(&now);

    result.year = 1900 + utc_time->tm_year;
    result.month = 1 + utc_time->tm_mon;
    result.day = utc_time->tm_mday;
    result.hour = utc_time->tm_hour;
    result.minute = utc_time->tm_min;
    result.second = utc_time->tm_sec;

    return result;
}

DateTime DateTime::from_iso_string(const char* iso_string) {
    DateTime result;
    int items_parsed = sscanf(iso_string, "%d-%d-%dT%d:%d:%dZ",
        &result.year, &result.month, &result.day, &result.hour, &result.minute, &result.second);
    if (items_parsed != 6) {
        throw exceptionf("ISO string %s is not valid.", iso_string);
    }

    return result;
}

std::string DateTime::to_iso_string() const {
    char buffer[26];
    sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02dZ",
        year, month, day, hour, minute, second);
    return std::string(buffer);
}
