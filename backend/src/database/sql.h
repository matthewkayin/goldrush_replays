#pragma once

#include <string>
#include <vector>
#include <variant>

struct sqlite3;
struct sqlite3_stmt;

using SqlBlob = std::vector<char>;
using SqlNull = std::monostate;
using SqlCell = std::variant<SqlNull, int64_t, double, std::string, SqlBlob>;
using SqlRow = std::vector<SqlCell>;
using SqlResult = std::vector<SqlRow>;

class SqlStatement {
public:
    SqlStatement(sqlite3* connection, const char* statement_str);
    ~SqlStatement();

    void bind_blob(int index, void* data, int length);
    void bind_null(int index);
    void bind_text(int index, const char* value);
    void bind_int(int index, int value);
    void bind_double(int index, double value);

    SqlResult execute();
private:
    sqlite3* m_connection;
    sqlite3_stmt* m_statement;
};

class SqlConnection {
public:
    SqlConnection();
    ~SqlConnection();

    SqlStatement prepare(const char* statement_str);
    SqlStatement prepare_from_file(const char* path);
private:
    sqlite3* m_connection;
};
