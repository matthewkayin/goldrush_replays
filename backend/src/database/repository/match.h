#pragma once

#include <crow.h>
#include <cstdint>
#include <string>
#include <vector>

struct MatchGetQueryParams {
    std::string ids;
    std::string date_gte;
    std::string date_lte;
    std::string name_contains;
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

std::vector<MatchGetRecord> match_repository_get(const MatchGetQueryParams& params);
std::vector<uint32_t> match_repository_post(std::vector<MatchPostRecord> records);
