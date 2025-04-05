#pragma once
#include <unordered_map>
#include <string>
#include "../Table/Table.h"
using namespace MiniDb::Table;

namespace MiniDb::Database {

    class Database {
    public:
        bool loadAllTables(const std::string& path);
        Table* getTable(const std::string& tableName);
        void listTables() const;

    private:
        std::unordered_map<std::string, Table> tables;
    };

}