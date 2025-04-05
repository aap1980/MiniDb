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
            std::cerr << "Błąd otwarcia pliku: " << filename << std::endl;
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
                    std::cerr << "Błąd: Niepoprawna linia w pliku. Oczekiwano 'Column'." << std::endl;
                    return false;
                }

                std::getline(ss, attributeName, ':');
                if (attributeName != "Name") {
                    std::cerr << "Błąd: Oczekiwano 'Name' w definicji kolumny." << std::endl;
                    return false;
                }
                std::getline(ss, attributeValue, SEP);
                columnName = attributeValue;

                std::getline(ss, attributeName, ':');
                if (attributeName != "Type") {
                    std::cerr << "Błąd: Oczekiwano 'Type' w definicji kolumny." << std::endl;
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
