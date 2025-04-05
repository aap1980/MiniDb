#pragma once
#include <string>
#include <vector>

namespace MiniDb::Metadata {

    class Column {
    public:
        std::string name;
        std::string type;

        Column(const std::string& name, const std::string& type);
    };

    class TableMetadataReader {
    private:
        constexpr static char SEP = 0x1F;
    public:
        std::string tableName;
        std::vector<Column> columns;

        TableMetadataReader(const std::string& tableName);
        bool loadFromFile(const std::string& filename);
    };

}
