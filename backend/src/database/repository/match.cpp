#include "match.h"

#include "logger.h"
#include "database/sql.h"
#include "api/util.h"

crow::json::wvalue MatchGetRecord::to_json() const {
    crow::json::wvalue json;
    json["id"] = id;
    json["name"] = name;
    json["date"] = date;
    json["duration"] = duration;

    return json;
}

std::string MatchGetQueryParams::to_where_clause() const {
    std::string where_clause = "1 = 1";

    // TODO: move validation out of this function

    // IDs
    if (!ids.empty()) {
        if (!api_query_string_validate_uint_list(ids.c_str())) {
            throw exceptionf("Query string param ids is not a list of numbers.");
        }
        where_clause += "\n and id in (" + ids + ")";
    }

    // Date from
    if (!date_from.empty()) {
        if (!api_query_string_validate_date_string(date_from.c_str())) {
            throw exceptionf("Query string param date_from is not a valid date.");
        }
        where_clause += "\n and date >= '" + date_from + "'";
    }

    // Date to
    if (!date_to.empty()) {
        if (!api_query_string_validate_date_string(date_to.c_str())) {
            throw exceptionf("Query string param date_to is not a valid date.");
        }
        where_clause += "\n and date <= '" + date_to + "'";
    }

    // Name
    if (!name_contains.empty()) {
        where_clause += "\n and name like '%" + name_contains + "%'";
    }

    return where_clause;
}

uint32_t match_repository_get_count(const MatchGetQueryParams& params) {
    std::string statement_str = "select count(*) as match_count from match where " + params.to_where_clause() + ";";

    SqlConnection connection;
    SqlStatement statement = connection.prepare(statement_str.c_str());

    SqlResult result = statement.execute();
    if (result.empty() || result[0].empty()) {
        throw exceptionf("Failed to determine total row count.");
    }

    return (uint32_t)std::get<int64_t>(result[0][0]);
}

std::vector<MatchGetRecord> match_repository_get(const MatchGetQueryParams& params, uint32_t offset, uint32_t limit) {
    std::string statement_str =
        "select id, name, date, duration from match where " + params.to_where_clause();
    if (limit != MATCH_GET_LIMIT_NONE) {
        statement_str += " limit " + std::to_string(limit);
    }
    // SQLite cannot do an offset unless you provide a limit
    if (limit != MATCH_GET_LIMIT_NONE && offset != MATCH_GET_OFFSET_NONE) {
        statement_str += " offset " + std::to_string(offset);
    }
    statement_str += ";";

    SqlConnection connection;
    SqlStatement statement = connection.prepare(statement_str.c_str());
    SqlResult result = statement.execute();

    std::vector<MatchGetRecord> records;
    for (const SqlRow& row : result) {
        records.push_back({
            .id = (uint32_t)std::get<int64_t>(row[0]),
            .name = std::get<std::string>(row[1]),
            .date = std::get<std::string>(row[2]),
            .duration = std::get<std::string>(row[3])
        });
    }

    return records;
}

std::vector<uint32_t> match_repository_post(std::vector<MatchPostRecord> records) {
    SqlConnection connection;
    SqlStatement statement = connection.prepare(
        "insert into match (name, date, duration, data) \
         values (?, ?, ?, ?) \
         returning id;");

    std::vector<uint32_t> ids;
    for (const MatchPostRecord& record : records) {
        statement.bind_text(1, record.name.c_str());
        statement.bind_text(2, record.date.c_str());
        statement.bind_text(3, record.duration.c_str());
        statement.bind_blob(4, (void*)record.data.data(), record.data.length());
        SqlResult result = statement.execute();

        if (result.empty()) {
            throw exceptionf("No ID returned after match insert.");
        }

        ids.push_back((uint32_t)std::get<int64_t>(result[0][0]));
    }

    return ids;
}
