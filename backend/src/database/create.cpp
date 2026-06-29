#include "create.h"

#include "logger.h"
#include "database/sql.h"

bool database_create() {
    Logger& logger = Logger::get_instance();
    logger.info("Creating database...");

    try {
        SqlConnection connection;
        SqlStatement create_table_match_statement = connection.prepare_from_file("../sql/create_table_match.sql");
        SqlStatement::Result result = create_table_match_statement.execute();

        return true;
    } catch (const std::exception& e) {
        logger.error("Exception in database_create() - %s", e.what());
        return false;
    }
}
