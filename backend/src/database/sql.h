#pragma once

#include <string>
#include <vector>

struct sqlite3;

class SqlStatement {
public:
    SqlStatement() = default;
    static SqlStatement from_string(const char* statement_str);
    static SqlStatement from_file(const char* path);

    void bind_null();
    void bind_integer(int value);
    void bind_real(double value);
    void bind_text(const char* value);

    std::string to_string() const;
private:
    std::string m_statement;
    std::vector<std::string> m_params;
};

class SqlConnection {
public:
    using QueryResult = std::vector<std::vector<std::string>>;

    SqlConnection();
    ~SqlConnection();
    QueryResult execute(const SqlStatement& statement);
private:
    sqlite3* m_connection;
};
