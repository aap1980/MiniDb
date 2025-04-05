#include "TableMetadataWriter.h"
#include <fstream>

using namespace MiniDb::Metadata;

ColumnDef::ColumnDef(const std::string& name, const std::string& type)
    : name(name), type(type) {
}

TableMetadataWriter::TableMetadataWriter(const std::string& name)
    : tableName(name) {
}

void TableMetadataWriter::addColumn(const std::string& name, const std::string& type) {
    columns.emplace_back(name, type);
}

bool TableMetadataWriter::saveToFile(const std::string& filename) const {
    std::ofstream out(filename, std::ios::out | std::ios::trunc);
    if (!out.is_open()) return false;

    out << "Table" << SEP << "Name:" << tableName << "\n";
    for (const auto& col : columns) {
        out << "Column" << SEP
            << "Name:" << col.name << SEP
            << "Type:" << col.type << "\n";
    }

    out.close();
    return true;
}
