#pragma once
#include <vector>
#include <string>
#include "../Metadata/TableMetadataReader.h"
using namespace MiniDb::Metadata;

namespace MiniDb::Table {

    class Table {
    public:
        std::string tableName;
        std::vector<Column> columns;
        std::vector<std::vector<std::string>> rows;  // Przechowuje dane wierszy jako wektory string�w

        Table(const std::string& tableName);
        bool loadMetadataFromFile(const std::string& metadataFile);
        void addRow(const std::vector<std::string>& row);
        void printTable() const;
    };

}
