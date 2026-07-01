#pragma once

#include <crow.h>
#include <cstdint>
#include <string>
#include <vector>
#include "database/sql.h"

const uint32_t REPLAY_GET_LIMIT_NONE = UINT32_MAX;
const uint32_t REPLAY_GET_OFFSET_NONE = UINT32_MAX;

struct ReplayGetQueryParams {
    std::string ids;
    std::string date_from;
    std::string date_to;
    std::string name_contains;

    std::string to_where_clause() const;
};

struct ReplayGetRecord {
    uint32_t id;
    std::string name;
    std::string date;
    std::string duration;

    crow::json::wvalue to_json() const;
};

struct ReplayPostRecord {
    std::string name;
    std::string date;
    std::string duration;
    std::string data;
};

struct ReplayDataRecord {
    uint32_t id;
    std::string name;
    SqlBlob data;
};

uint32_t replay_repository_get_count(const ReplayGetQueryParams& params);
std::vector<ReplayGetRecord> replay_repository_get(const ReplayGetQueryParams& params, uint32_t offset, uint32_t limit);
std::vector<uint32_t> replay_repository_post(std::vector<ReplayPostRecord> records);
ReplayDataRecord replay_repository_get_data(uint32_t replay_id);
