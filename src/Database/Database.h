#pragma once
#include <unordered_map>
#include <string>
#include "../Table/Table.h"

namespace MiniDb::Database {

    class Database {
    public:
        bool loadAllTables(const std::string& path);
        MiniDb::Table::Table* getTable(const std::string& tableName);
        void listTables() const;

    private:
        std::unordered_map<std::string, MiniDb::Table::Table> tables;
    };

}