#include "match.h"

#include "logger.h"
#include "database/sql.h"
#include "util/qs_validate.h"

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

    // IDs
    if (!params.ids.empty()) {
        if (!query_string_validate_uint_list(params.ids.c_str())) {
            throw exceptionf("Query string param ids is not a list of numbers.");
        }
        statement_str += "\n and id in (" + params.ids + ")";
    }

    // Date after
    if (!params.date_gte.empty()) {
        if (!query_string_validate_date_iso_string(params.date_gte.c_str())) {
            throw exceptionf("Query string param start_date is not a valid date.");
        }
        statement_str += "\n and date >= '" + params.date_gte + "'";
    }

    // End date
    if (!params.date_lte.empty()) {
        if (!query_string_validate_date_iso_string(params.date_lte.c_str())) {
            throw exceptionf("Query string param end_date is not a valid date.");
        }
        statement_str += "\n and date <= '" + params.date_lte + "'";
    }

    // Name
    if (!params.name_contains.empty()) {
        statement_str += "\n and name like '%" + params.name_contains + "%'";
    }

    statement_str += ";";

    SqlConnection connection;
    SqlStatement statement = connection.prepare(statement_str.c_str());
    SqlStatement::Results results = statement.execute();

    std::vector<MatchGetRecord> records;
    for (const auto& row : results) {
        records.push_back({
            .id = stoul(row[0]),
            .name = row[1],
            .date = row[2],
            .duration = row[3]
        });
    }

    return records;
}

std::vector<MatchGetRecord> match_repository_get_by_ids(const std::vector<uint32_t>& ids) {
    std::string id_list;
    for (uint32_t id_index = 0; id_index < ids.size(); id_index++) {
        if (id_index != 0) {
            id_list += ",";
        }
        id_list += std::to_string(ids[id_index]);
    }

    SqlConnection connection;
    SqlStatement statement = connection.prepare(
        (std::string("select id, name, date, duration from match \
         where id in (") + id_list + ");").c_str());
    SqlStatement::Results results = statement.execute();

    std::vector<MatchGetRecord> records;
    for (const auto& row : results) {
        records.push_back({
            .id = stoul(row[0]),
            .name = row[1],
            .date = row[2],
            .duration = row[3]
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
        SqlStatement::Results results = statement.execute();

        if (results.empty()) {
            throw exceptionf("No ID returned after match insert.");
        }

        ids.push_back(stoul(results[0][0]));
    }

    return ids;
}
