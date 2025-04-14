#include "SelectStatement.h"
#include <iostream>
#include "../Database/Database.h"
#include "../SqlParser/SQLParser.h"

namespace MiniDb::Statement {

    SelectStatement::SelectStatement(const hsql::SelectStatement* stmt) {
        if (stmt->fromTable != nullptr)
            tableName_ = stmt->fromTable->getName();

        selectAll_ = stmt->selectList->size() == 1 &&
            (*stmt->selectList)[0]->type == hsql::kExprStar;
    }

    void SelectStatement::execute(MiniDb::Database::Database& db) const {
        const MiniDb::Table::Table& table = db.getTable(tableName_);

        if (!selectAll_) {
            std::cout << "Obs³uga innych kolumn ni¿ * niezaimplementowana.\n";
            return;
        }

        const auto& columns = table.metadata.columns;
        const auto& rows = table->getAllRows();

        for (const auto& col : columns)
            std::cout << col.name << "\t";
        std::cout << "\n";

        for (const auto& row : rows) {
            for (const auto& val : row)
                std::cout << val << "\t";
            std::cout << "\n";
        }
    }

}
