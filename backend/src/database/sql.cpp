#include "sql.h"

#define SQLITE_THREADSAFE 1

#include "logger.h"
#include "sqlite/sqlite3.h"
#include <fstream>

static const char* DATABASE_FILE = "./database.db";

SqlStatement::SqlStatement(sqlite3* connection, const char* statement_str) {
    m_connection = connection;

    // Prepare statement
    int result = sqlite3_prepare_v2(connection, statement_str, strlen(statement_str) + 1, &m_statement, nullptr);
    if (result != SQLITE_OK) {
        throw exceptionf("Error preparing SQL statement: %s", sqlite3_errmsg(m_connection));
    }
}

SqlStatement::~SqlStatement() {
    if (m_statement != nullptr) {
        int result = sqlite3_finalize(m_statement);
        if (result != SQLITE_OK) {
            Logger& logger = Logger::get_instance();
            logger.warn("Non-OK result returned in sqlite3_finalize in ~SqlStatement. Code: %i Message: %s",
                result, sqlite3_errmsg(m_connection));
        }
    }
}

void SqlStatement::bind_blob(int index, void* data, int length) {
    sqlite3_bind_blob(m_statement, index, data, length, SQLITE_STATIC);
}

void SqlStatement::bind_null(int index) {
    sqlite3_bind_null(m_statement, index);
}

void SqlStatement::bind_text(int index, const char* value) {
    sqlite3_bind_text(m_statement, index, value, strlen(value) + 1, SQLITE_STATIC);
}

void SqlStatement::bind_int(int index, int value) {
    sqlite3_bind_int(m_statement, index, value);
}

void SqlStatement::bind_double(int index, double value) {
    sqlite3_bind_double(m_statement, index, value);
}

SqlStatement::Results SqlStatement::execute() {
    Logger& logger = Logger::get_instance();
    Results result_rows;
    int result_code;

    // Execute statement
    while ((result_code = sqlite3_step(m_statement)) != SQLITE_DONE) {
        switch (result_code) {
            case SQLITE_ROW: {
                result_rows.push_back(std::vector<std::string>());
                for (int col = 0; col < sqlite3_column_count(m_statement); col++) {
                    unsigned const char* col_text = sqlite3_column_text(m_statement, col);
                    if (!col_text) {
                        result_rows.back().push_back("NULL");
                    } else {
                        std::string result_str = std::string((const char*)col_text);
                        result_rows.back().push_back(result_str);
                    }
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
                throw exceptionf("SQL execute encountered error. Code: %i Message %s",
                    result_code, sqlite3_errmsg(m_connection));
                break;
            }
        }
    }

    // Reset the statement
    result_code = sqlite3_reset(m_statement);
    if (result_code != SQLITE_OK) {
        throw exceptionf("SqlStatement::execute() sqlite3_reset() failed. Code: %i Message: %s",
            result_code, sqlite3_errmsg(m_connection));
    }

    return result_rows;
}

SqlConnection::SqlConnection() {
    int result = sqlite3_open(DATABASE_FILE, &m_connection);
    if (result != SQLITE_OK) {
        throw exceptionf("Error opening database: %s", sqlite3_errmsg(m_connection));
    }
}

SqlConnection::~SqlConnection() {
    sqlite3_close(m_connection);
}

SqlStatement SqlConnection::prepare(const char* statement_str) {
    return SqlStatement(m_connection, statement_str);
}

SqlStatement SqlConnection::prepare_from_file(const char* path) {
    std::ifstream sql_file(path);
    if (!sql_file) {
        throw exceptionf("Unable to read SQL statement at path %s", path);
    }

    std::ostringstream string_stream;
    string_stream << sql_file.rdbuf();

    return SqlStatement(m_connection, string_stream.str().c_str());
}
