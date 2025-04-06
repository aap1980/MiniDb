#include <iostream>
#include <fstream>
#include "Table.h"
#include "../Constants.h"
#include "../Metadata/TableMetadataReader.h"
#include "../Config/Config.h"

namespace MiniDb::Table {

	Table::Table(const std::string& tableName)
		: tableName(tableName) {
		std::string tablesPath = MiniDb::Config::Config::getInstance().getTablesPath();
		metadataFile = tablesPath + tableName + ".md";
		dataFile = tablesPath + tableName + ".dat";
		loadMetadata(metadataFile);
		loadData(dataFile);
		std::cout << "Successfully loaded table: " << tableName << "\n";
	}

	bool Table::loadMetadata(const std::string& metadataFile) {
		MiniDb::Metadata::TableMetadataReader reader(tableName);
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
		saveDataToFile(dataFile);
	}

	bool Table::saveDataToFile(const std::string& filename) const {
		std::ofstream file(filename, std::ios::app);
		if (!file.is_open()) {
			std::cerr << "Error opening file: " << filename << std::endl;
			return false;
		}

		for (const auto& row : rows) {
			for (const auto& data : row) {
				file << data << static_cast<char>(MiniDb::SEP);
			}
			file << '\n';
		}

		file.close();
		return true;
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
