#include "replay.h"

#include "logger.h"
#include "database/sql.h"
#include "api/util.h"

crow::json::wvalue ReplayGetRecord::to_json() const {
    crow::json::wvalue json;
    json["id"] = id;
    json["name"] = name;
    json["date"] = date;
    json["duration"] = duration;

    return json;
}

std::string ReplayGetQueryParams::to_where_clause() const {
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

uint32_t replay_repository_get_count(const ReplayGetQueryParams& params) {
    std::string statement_str = "select count(*) as replay_count from replays where " + params.to_where_clause() + ";";

    SqlConnection connection;
    SqlStatement statement = connection.prepare(statement_str.c_str());

    SqlResult result = statement.execute();
    if (result.empty() || result[0].empty()) {
        throw exceptionf("Failed to determine total row count.");
    }

    return (uint32_t)std::get<int64_t>(result[0][0]);
}

std::vector<ReplayGetRecord> replay_repository_get(const ReplayGetQueryParams& params, uint32_t offset, uint32_t limit) {
    std::string statement_str =
        "select id, name, date, duration from replays where " + params.to_where_clause();
    if (limit != REPLAY_GET_LIMIT_NONE) {
        statement_str += " limit " + std::to_string(limit);
    }
    // SQLite cannot do an offset unless you provide a limit
    if (limit != REPLAY_GET_LIMIT_NONE && offset != REPLAY_GET_OFFSET_NONE) {
        statement_str += " offset " + std::to_string(offset);
    }
    statement_str += ";";

    SqlConnection connection;
    SqlStatement statement = connection.prepare(statement_str.c_str());
    SqlResult result = statement.execute();

    std::vector<ReplayGetRecord> records;
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

std::vector<uint32_t> replay_repository_post(std::vector<ReplayPostRecord> records) {
    SqlConnection connection;
    SqlStatement statement = connection.prepare(
        "insert into replays (name, date, duration, data) \
         values (?, ?, ?, ?) \
         returning id;");

    std::vector<uint32_t> ids;
    for (const ReplayPostRecord& record : records) {
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

ReplayDataRecord replay_repository_get_data(uint32_t replay_id) {
    Logger& logger = Logger::get_instance();

    SqlConnection connection;
    SqlStatement statement = connection.prepare("select name, data from replays where id = ?;");
    statement.bind_int(1, replay_id);

    SqlResult result = statement.execute();
    if (result.empty()) {
        throw exceptionf("Replay with ID %u does not exist.", replay_id);
    }
    if (result.size() > 1) {
        logger.warn("Multiple rows returned for match with ID %u. Row count: %u", replay_id, result.size());
    }

    return {
        .id = replay_id,
        .name = std::get<std::string>(result[0][0]),
        .data = std::get<SqlBlob>(result[0][1])
    };
}
