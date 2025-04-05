#include "TableMetadataReader.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace MiniDb::Metadata {

    Column::Column(const std::string& name, const std::string& type)
        : name(name), type(type) {
    }

    TableMetadataReader::TableMetadataReader(const std::string& tableName)
        : tableName(tableName) {
    }

    bool TableMetadataReader::loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "B³¹d otwarcia pliku: " << filename << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.find("Table") != std::string::npos) {
                // Sprawdzamy, czy nazwa tabeli pasuje
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
                    std::cerr << "B³¹d: Niepoprawna linia w pliku. Oczekiwano 'Column'." << std::endl;
                    return false;
                }

                std::getline(ss, attributeName, ':');
                if (attributeName != "Name") {
                    std::cerr << "B³¹d: Oczekiwano 'Name' w definicji kolumny." << std::endl;
                    return false;
                }
                std::getline(ss, attributeValue, SEP);
                columnName = attributeValue;

                std::getline(ss, attributeName, ':');
                if (attributeName != "Type") {
                    std::cerr << "B³¹d: Oczekiwano 'Type' w definicji kolumny." << std::endl;
                    return false;
                }
                std::getline(ss, attributeValue, SEP);
                columnType = attributeValue;

                columns.emplace_back(columnName, columnType);
            }
        }

        file.close();
        return true;
    }

}
