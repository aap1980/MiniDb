#include <fstream>
#include <sstream>
#include <iostream>
#include "../Constants.h"
#include "../Config/Config.h"
#include "TableMetadata.h"

namespace MiniDb::Table {

	void TableMetadata::initialize(const std::string tableName) {
		std::string tablesPath = MiniDb::Config::Config::getInstance().getTablesPath();
		_filename = tablesPath + tableName + ".md";
		loadFromFile();
	}

	void TableMetadata::loadFromFile() {
		std::ifstream file(_filename);
		if (!file.is_open()) {
			throw std::runtime_error("Unable to open file: " + _filename);
		}

		std::string line;
		while (std::getline(file, line)) {
			if (line.find("Table") != std::string::npos) {
				if (line.find(_tableName) == std::string::npos) {
					continue;
				}
			}
			else if (line.find("Column") != std::string::npos) {
				std::istringstream ss(line);
				std::string objectName, attributeName, attributeValue;
				std::string columnName, columnType;

				std::getline(ss, objectName, SEP);
				if (objectName != "Column") {
					throw std::runtime_error("Invalid line in file (expected 'Column'): " + line);
				}

				std::getline(ss, attributeName, ':');
				if (attributeName != "Name") {
					throw std::runtime_error("Expected 'Name' in column definition: " + line);
				}
				std::getline(ss, attributeValue, SEP);
				columnName = attributeValue;

				std::getline(ss, attributeName, ':');
				if (attributeName != "Type") {
					throw std::runtime_error("Expected 'Type' in column definition: " + line);
				}
				std::getline(ss, attributeValue, SEP);
				columnType = attributeValue;

				MiniDb::Table::Column column(columnName, columnType);
				addColumn(column);
			}
		}
	}

	void TableMetadata::saveToFile() {
		std::ofstream out(_filename, std::ios::out | std::ios::trunc);
		if (!out.is_open()) {
			throw std::runtime_error("Unable to open file: " + _filename);
		}

		out << "Table" << SEP << "Name:" << _tableName << "\n";
		for (const auto& col : columns) {
			out << "Column" << SEP
				<< "Name:" << col.name << SEP
				<< "Type:" << col.type << "\n";
		}

		out.close();
	}

	void TableMetadata::addColumn(const MiniDb::Table::Column column) {
		columns.push_back(column);
	}

	void TableMetadata::addColumns(const std::vector<Column>& newColumns) {
		columns.insert(columns.end(), newColumns.begin(), newColumns.end());
	}

}
