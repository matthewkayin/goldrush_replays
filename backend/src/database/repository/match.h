#pragma once

#include <crow.h>
#include <cstdint>
#include <string>
#include <vector>

const uint32_t MATCH_GET_LIMIT_NONE = UINT32_MAX;
const uint32_t MATCH_GET_OFFSET_NONE = UINT32_MAX;

struct MatchGetQueryParams {
    std::string ids;
    std::string date_from;
    std::string date_to;
    std::string name_contains;

    std::string to_where_clause() const;
};

struct MatchGetRecord {
    uint32_t id;
    std::string name;
    std::string date;
    std::string duration;

    crow::json::wvalue to_json() const;
};

struct MatchPostRecord {
    std::string name;
    std::string date;
    std::string duration;
    std::string data;
};

uint32_t match_repository_get_count(const MatchGetQueryParams& params);
std::vector<MatchGetRecord> match_repository_get(const MatchGetQueryParams& params, uint32_t offset, uint32_t limit);
std::vector<uint32_t> match_repository_post(std::vector<MatchPostRecord> records);
