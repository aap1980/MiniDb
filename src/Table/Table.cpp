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
		std::cout << "Successfully loaded table: " << tableName << "\n";
	}

	bool Table::loadMetadata(const std::string& metadataFile) {
		MiniDb::Metadata::TableMetadataReader reader(tableName);
		return reader.loadFromFile(metadataFile, *this);
	}

	void Table::addRow(const std::vector<std::string>& row) {
		if (row.size() != columns.size()) {
			std::cerr << "Error: the number of data in a row does not match the number of columns.\n";
			return;
		}
		std::vector<std::vector<std::string>> rows = { row };
		saveDataToFile(dataFile, rows);
	}

	bool Table::saveDataToFile(const std::string& filename, const std::vector<std::vector<std::string>>& rows) const {
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

	void Table::selectAll() const {
		std::ifstream file(dataFile);
		if (!file.is_open()) {
			std::cerr << "Error opening data file: " << dataFile << std::endl;
			return;
		}

		std::string line;
		while (std::getline(file, line)) {
			std::vector<std::string> row;
			size_t pos = 0;
			while ((pos = line.find(static_cast<char>(MiniDb::SEP))) != std::string::npos) {
				row.push_back(line.substr(0, pos));
				line.erase(0, pos + 1);
			}
			row.push_back(line);
			for (const auto& data : row) {
				std::cout << data << "\t";
			}
			std::cout << "\n";
		}

		file.close();
	}

	void Table::printTable() const {
		std::cout << "Tabela: " << tableName << "\n";
		for (const auto& column : columns) {
			std::cout << column.name << "\t";
		}
		std::cout << "\n";
		selectAll();
	}

}
