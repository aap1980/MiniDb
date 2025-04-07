#pragma once
#include <vector>
#include <string>
#include "../Database/Database.h"
#include "../Parser/AST.h"

namespace MiniDb::Executor {

    class SelectExecutor {
    private:
        MiniDb::Database::Database& database;

    public:
        explicit SelectExecutor(MiniDb::Database::Database& database);
        std::vector<std::vector<std::string>> execute(const MiniDb::Parser::SelectStatement& statement);
    };

}
