#include "create.h"

#include "logger.h"
#include "database/sql.h"

bool database_create() {
    Logger& logger = Logger::get_instance();
    logger.info("Creating database...");

    try {
        SqlConnection connection;
        SqlStatement create_table_match_statement = SqlStatement::from_file("../sql/create_table_match.sql");
        SqlConnection::QueryResult result = connection.execute(create_table_match_statement);

        return true;
    } catch (const std::exception& e) {
        logger.error((std::string("Exception in database_create() - ") + e.what()).c_str());
        return false;
    }
}
