#include <fstream>
#include <iostream>
#include "..\Constants.h"
#include "TableDataWriter.h"

namespace MiniDb::Table {

    TableDataWriter::TableDataWriter(const std::string& name)
        : _tableName(name) {
    }

    void TableDataWriter::addRow(const std::vector<std::string>& row) {
        rows.push_back(row);
    }

    bool TableDataWriter::saveToFile(const std::string& _filename) const {
        std::ofstream file(_filename, std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error opening file for writing: " << _filename << std::endl;
            return false;
        }

        for (const auto& row : rows) {
            for (size_t i = 0; i < row.size(); ++i) {
                file << row[i];
                if (i < row.size() - 1) {
                    file.put(SEP);
                }
            }
            file.put('\n');
        }

        file.close();
        return true;
    }
}
