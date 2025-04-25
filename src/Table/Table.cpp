#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "Row.h"
#include "Rows.h"
#include "Table.h"
#include "../Constants/Constants.h"
#include "../Config/Config.h"
#include "../ErrorHandling.h"
#include "filesystem"

namespace MiniDb::Table {

	Table::Table(const std::string& tableName)
		: _tableName(tableName), rows() {
		std::string tablesPath = MiniDb::Config::Config::getInstance().getTablesPath();
		_dataFile = tablesPath + tableName + ".dat";
		_metadataFile = tablesPath + tableName + ".md";
		loadMetadataFromFile();
	}

	void Table::loadMetadataFromFile() {
		if (!std::filesystem::exists(_metadataFile)) return;

		std::ifstream file(_metadataFile);
		if (!file.is_open()) {
			throw FileWriteException("Error opening data file: " + _metadataFile);
		}

		std::string line;
		while (std::getline(file, line)) {
			if (line.find("Column") != std::string::npos) {
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
				columns.addColumn(column);
			}
		}
	}

	void Table::saveMetadataToFile() {
		std::filesystem::path filePath = _metadataFile;
		std::filesystem::create_directories(filePath.parent_path());

		std::ofstream file(_metadataFile, std::ios::out | std::ios::trunc);
		if (!file.is_open()) {
			throw std::runtime_error("Unable to create or open file: " + _metadataFile);
		}

		for (const auto& col : columns.getColumns()) {
			file << "Column" << SEP
				<< "Name:" << col.name << SEP
				<< "Type:" << col.type << "\n";
		}

		file.close();
	}

	void Table::loadDataFromFile() {
		rows.clear();

		if (!std::filesystem::exists(_dataFile)) return;

		std::ifstream file(_dataFile);
		if (!file) {
			throw FileWriteException("Error opening data file: " + _dataFile);
		}

		std::string line;
		while (std::getline(file, line)) {
			std::vector<std::string> rowData;
			std::stringstream ss(line);
			std::string cell;

			while (std::getline(ss, cell, static_cast<char>(MiniDb::SEP))) {
				rowData.push_back(cell);
			}

			rows.addRow(Row(std::move(rowData)));
		}
	}

	void Table::saveDataToFile() {
		std::filesystem::path filePath = _dataFile;
		std::filesystem::create_directories(filePath.parent_path());

		std::ofstream file(_dataFile, std::ios::out | std::ios::trunc);
		if (!file.is_open()) {
			throw std::runtime_error("Unable to create or open file: " + _dataFile);
		}

		for (const auto& row : rows.getRows()) {
			for (const auto& value : row.getValues()) {
				file << value << static_cast<char>(MiniDb::SEP);
			}
			file << '\n';
		}

		file.close();
	}

}
