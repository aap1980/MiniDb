#include <iostream>
#include <fstream>
#include "Table.h"
#include "../Metadata/TableMetadataReader.h"
using namespace MiniDb::Metadata;

namespace MiniDb::Table {

    Table::Table(const std::string& tableName)
        : tableName(tableName) {
    }

    bool Table::loadMetadata(const std::string& metadataFile) {
        TableMetadataReader reader(tableName);
        return reader.loadFromFile(metadataFile, *this);
    }

    bool Table::loadData(const std::string& dataFile) {
        return true;
    }

    void Table::addRow(const std::vector<std::string>& row) {
        if (row.size() != columns.size()) {
            std::cerr << "Error: the number of data in a row does not match the number of columns.\n";
            return;
        }
        rows.push_back(row);
    }

    void Table::printTable() const {
        std::cout << "Tabela: " << tableName << "\n";
        for (const auto& column : columns) {
            std::cout << column.name << "\t";
        }
        std::cout << "\n";

        for (const auto& row : rows) {
            for (const auto& data : row) {
                std::cout << data << "\t";
            }
            std::cout << "\n";
        }
    }

}
