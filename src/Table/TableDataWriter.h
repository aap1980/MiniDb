#pragma once
#include <string>
#include <vector>

namespace MiniDb::Table {

    class TableDataWriter {
    private:
        std::string _tableName;
        std::vector<std::vector<std::string>> rows;

    public:
        TableDataWriter(const std::string& name);
        void addRow(const std::vector<std::string>& row);
        bool saveToFile(const std::string& _filename) const;
    };

}
