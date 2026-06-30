#include "sql.h"

#define SQLITE_THREADSAFE 1

#include "logger.h"
#include "sqlite/sqlite3.h"
#include <fstream>

static const char* DATABASE_FILE = "./database.db";

// Internal
const char* sql_sqlite_type_str(int type);

// STATEMENT

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

SqlResult SqlStatement::execute() {
    Logger& logger = Logger::get_instance();
    SqlResult result;
    int result_code;

    // Execute statement
    while ((result_code = sqlite3_step(m_statement)) != SQLITE_DONE) {
        switch (result_code) {
            case SQLITE_ROW: {
                SqlRow row;

                for (int col = 0; col < sqlite3_column_count(m_statement); col++) {
                    int type = sqlite3_column_type(m_statement, col);

                    switch (type) {
                        case SQLITE_INTEGER: {
                            row.push_back((int64_t)sqlite3_column_int64(m_statement, col));
                            break;
                        }
                        case SQLITE_FLOAT: {
                            row.push_back(sqlite3_column_double(m_statement, col));
                            break;
                        }
                        case SQLITE_TEXT: {
                            const char* text = reinterpret_cast<const char*>(sqlite3_column_text(m_statement, col));
                            row.push_back(text ? std::string(text) : std::string("NULL"));
                            break;
                        }
                        case SQLITE_BLOB: {
                            const uint8_t* blob_data = (uint8_t*)sqlite3_column_blob(m_statement, col);
                            int blob_size = sqlite3_column_bytes(m_statement, col);

                            SqlBlob blob(blob_data, blob_data + blob_size);
                            row.push_back(std::move(blob));
                            break;
                        }
                        case SQLITE_NULL:
                        default: {
                            row.push_back(SqlNull{});
                            break;
                        }
                    }
                }

                result.push_back(std::move(row));
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

    return result;
}

// CONNECTION

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

// INTERNAL

const char* sql_sqlite_type_str(int type) {
    switch (type) {
        case SQLITE_INTEGER:
            return "INTEGER";
        case SQLITE_FLOAT:
            return "FLOAT";
        case SQLITE_TEXT:
            return "TEXT";
        case SQLITE_BLOB:
            return "BLOB";
        case SQLITE_NULL:
            return "NULL";
        default:
            return "UNKNOWN";
    }
}
