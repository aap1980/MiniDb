#include <fstream>
#include <sstream>
#include <iostream>
#include "TableMetadataReader.h"
#include "../Table/Table.h"

namespace MiniDb::Metadata {

    TableMetadataReader::TableMetadataReader(const std::string& tableName)
        : tableName(tableName) {
    }

    bool TableMetadataReader::loadFromFile(const std::string& filename, MiniDb::Table::Table& table) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.find("Table") != std::string::npos) {
                if (line.find(tableName) == std::string::npos) {
                    continue;
                }
            }
            else if (line.find("Column") != std::string::npos) {
                std::istringstream ss(line);
                std::string objectName, attributeName, attributeValue;
                std::string columnName, columnType;

                std::getline(ss, objectName, SEP);
                if (objectName != "Column") {
                    std::cerr << "Error: Incorrect line in file. Expected  'Column'." << std::endl;
                    return false;
                }

                std::getline(ss, attributeName, ':');
                if (attributeName != "Name") {
                    std::cerr << "Error: Expected 'Name' in column definition." << std::endl;
                    return false;
                }
                std::getline(ss, attributeValue, SEP);
                columnName = attributeValue;

                std::getline(ss, attributeName, ':');
                if (attributeName != "Type") {
                    std::cerr << "Error: Expected 'Type' in column definition." << std::endl;
                    return false;
                }
                std::getline(ss, attributeValue, SEP);
                columnType = attributeValue;

                table.columns.emplace_back(columnName, columnType);
            }
        }

        file.close();
        return true;
    }

}
