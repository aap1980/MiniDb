#pragma once

#include "Statement.h"
#include "../Database/Database.h"
#include "../SqlParser/SQLParser.h"

namespace MiniDb::Statement {

    class SelectStatement : public Statement {
    public:
        SelectStatement(const hsql::SelectStatement* stmt);
        void execute(MiniDb::Database::Database& db) const override;

    private:
        std::string tableName_;
        bool selectAll_;
    };

}
