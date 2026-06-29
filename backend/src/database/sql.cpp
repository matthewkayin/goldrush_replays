#include "sql.h"

#define SQLITE_THREADSAFE 1

#include "logger.h"
#include "sqlite/sqlite3.h"
#include <fstream>

static const char* DATABASE_FILE = "./database.db";

SqlStatement SqlStatement::from_string(const char* statement_str) {
    SqlStatement out_statement;
    out_statement.m_statement = statement_str;
    return out_statement;
}

SqlStatement SqlStatement::from_file(const char* path) {
    std::ifstream sql_file(path);
    if (!sql_file) {
        throw std::exception((std::string("Unable to read SQL statement at path ") + path).c_str());
    }

    std::ostringstream string_stream;
    string_stream << sql_file.rdbuf();

    return SqlStatement::from_string(string_stream.str().c_str());
}

void SqlStatement::bind_null() {
    m_params.push_back(std::string("NULL"));
}

void SqlStatement::bind_integer(int value) {
    m_params.push_back(std::to_string(value));
}

void SqlStatement::bind_real(double value) {
    return m_params.push_back(std::to_string(value));
}

void SqlStatement::bind_text(const char* value) {
    m_params.push_back(std::string("'") + value + "'");
}

std::string SqlStatement::to_string() const {
    std::string result = m_statement;
    size_t question_mark_index = result.find('?');
    size_t param_index = 0;
    while (question_mark_index != std::string::npos) {
        if (param_index >= m_params.size()) {
            throw std::exception("Sql statement expects more parameters than are bound.");
        }

        result.replace(question_mark_index, 1, m_params[param_index]);
    }

    return result;
}

SqlConnection::SqlConnection() {
    int result = sqlite3_open(DATABASE_FILE, &m_connection);
    if (result != SQLITE_OK) {
        throw std::exception((std::string("Error opening database: ") + sqlite3_errmsg(m_connection)).c_str());
    }
}

SqlConnection::~SqlConnection() {
    sqlite3_close(m_connection);
}

SqlConnection::QueryResult SqlConnection::execute(const SqlStatement& statement) {
    QueryResult result_rows;

    Logger& logger = Logger::get_instance();
    const std::string statement_str = statement.to_string();

    // Prepare statement
    sqlite3_stmt* sqlite_statement;
    int result = sqlite3_prepare_v2(
        m_connection, statement_str.c_str(), statement_str.size() + 1, &sqlite_statement, nullptr);
    if (result != SQLITE_OK) {
        throw std::exception((std::string("Error preparing SQL statement: ") + sqlite3_errmsg(m_connection)).c_str());
    }

    // Execute statement
    while ((result = sqlite3_step(sqlite_statement)) != SQLITE_DONE) {
        switch (result) {
            case SQLITE_ROW: {
                result_rows.push_back(std::vector<std::string>());
                for (int col = 0; col < sqlite3_column_count(sqlite_statement); col++) {
                    result_rows.back().push_back(std::string((const char*)sqlite3_column_text(sqlite_statement, col)));
                }
                break;
            }
            case SQLITE_BUSY: {
                logger.warn("SQL execution received SQLITE_BUSY. Retrying...");
                // TODO: if statement is not a commit and we are in an explicit transition, rollback
                // Retry execution
                continue;
            }
            case SQLITE_MISUSE:
            case SQLITE_ERROR:
            default: {
                throw std::exception(
                    (std::string("SQL execute encountered error. Code: ") + std::to_string(result) +
                        " Message: " + sqlite3_errmsg(m_connection)).c_str());
                break;
            }
        }
    }

    // Finalize statement
    sqlite3_finalize(sqlite_statement);

    return result_rows;
}
