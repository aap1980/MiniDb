#include <iostream>
#include "SelectExecutor.h"

namespace MiniDb::Executor {

    SelectExecutor::SelectExecutor(MiniDb::Database::Database& database) : database(database) {}

    /*std::vector<std::vector<std::string>> SelectExecutor::execute(const MiniDb::Parser::SelectStatement& statement) {
        try {
            auto& table = database.getTable(statement.fromTable);

            //MiniDb::Table::QueryCondition condition;
            //statement.whereCondition.
            //condition.addCondition("id", "2");

            // TODO: filtrowanie kolumn i wierszy zgodnie z AST
            // Na razie tylko "select *"
            table.selectAll(); // je�li chcesz tylko wypisa�
            // Albo zwr�� dane jako vector<vector<string>> (musisz doda� tak� metod� w Table)

        }
        catch (const std::exception& ex) {
            std::cerr << "B��d podczas wykonania SELECT: " << ex.what() << std::endl;
        }

        return {};
    }*/

}
