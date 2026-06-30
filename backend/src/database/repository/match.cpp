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

std::vector<MatchGetRecord> match_repository_get(const MatchGetQueryParams& params) {
    // Convert query string into statement
    std::string statement_str = "select id, name, date, duration from match where 1 = 1";

    // TODO: move validation out of this function

    // IDs
    if (!params.ids.empty()) {
        if (!api_query_string_validate_uint_list(params.ids.c_str())) {
            throw exceptionf("Query string param ids is not a list of numbers.");
        }
        statement_str += "\n and id in (" + params.ids + ")";
    }

    // Date from
    if (!params.date_from.empty()) {
        if (!api_query_string_validate_date_string(params.date_from.c_str())) {
            throw exceptionf("Query string param date_from is not a valid date.");
        }
        statement_str += "\n and date >= '" + params.date_from + "'";
    }

    // Date to
    if (!params.date_to.empty()) {
        if (!api_query_string_validate_date_string(params.date_to.c_str())) {
            throw exceptionf("Query string param date_to is not a valid date.");
        }
        statement_str += "\n and date <= '" + params.date_to + "'";
    }

    // Name
    if (!params.name_contains.empty()) {
        statement_str += "\n and name like '%" + params.name_contains + "%'";
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
