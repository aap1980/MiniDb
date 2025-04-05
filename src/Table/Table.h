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
        std::vector<std::vector<std::string>> rows;  // Przechowuje dane wierszy jako wektory stringów

        Table(const std::string& tableName);
        bool loadMetadata(const std::string& metadataFile);
        bool loadData(const std::string& dataFile);
        void addRow(const std::vector<std::string>& row);
        void printTable() const;
    };

}
