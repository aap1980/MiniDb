#include <iostream>
#include <fstream>
#include <sstream>
#include "Table.h"
#include "../Constants.h"
#include "../Config/Config.h"
#include "TableMetadata.h"

namespace MiniDb::Table {

	Table::Table(const std::string& tableName)
		: tableName(tableName), metadata(tableName) {
		std::string tablesPath = MiniDb::Config::Config::getInstance().getTablesPath();
		dataFile = tablesPath + tableName + ".dat";
		std::cout << "Successfully loaded table: " << tableName << "\n";
	}

	bool Table::readDataFromFile(const std::string& filename, std::vector<std::vector<std::string>>& rows) const {
		std::ifstream file(filename);
		if (!file.is_open()) {
			std::cerr << "Error opening data file: " << filename << std::endl;
			return false;
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
			rows.push_back(row);
		}

		file.close();
		return true;
	}

	void Table::addRow(const std::vector<std::string>& row) {
		if (row.size() != metadata.columns.size()) {
			std::cerr << "Error: the number of data in a row does not match the number of columns.\n";
			return;
		}

		if (!writeRowToFile(row)) {
			std::cerr << "Error: failed to add row to file.\n";
		}
	}

	void Table::updateRow(const QueryCondition& condition, const std::vector<std::string>& newRow) {
		std::vector<std::vector<std::string>> rows;
		if (!readDataFromFile(dataFile, rows)) {
			return;
		}

		bool found = false;
		for (auto& row : rows) {
			bool match = true;

			const auto& conditions = condition.getConditions();
			size_t colIndex = 0;

			for (const auto& [column, value] : conditions) {
				auto matchLambda = [&row, &column, &value, &colIndex, this](bool& match) {
					if (metadata.columns[colIndex].name == column && row[colIndex] != value) {
						match = false;
					}
					};
				matchLambda(match);

				++colIndex;
			}

			if (match) {
				for (size_t i = 0; i < newRow.size(); ++i) {
					row[i] = newRow[i];
				}
				found = true;
				break;
			}
		}

		if (found) {
			saveDataToFile(dataFile, rows);
		}
		else {
			std::cerr << "Error: row not found for update.\n";
		}
	}

	void Table::deleteRow(const QueryCondition& condition) {
		std::vector<std::vector<std::string>> rows;
		if (!readDataFromFile(dataFile, rows)) {
			return;
		}

		bool found = false;
		auto it = rows.begin();

		while (it != rows.end()) {
			bool match = true;
			const auto& conditions = condition.getConditions();
			size_t colIndex = 0;

			for (const auto& [column, value] : conditions) {
				auto matchLambda = [&row = *it, &column, &value, &colIndex, this](bool& match) {
					if (metadata.columns[colIndex].name == column && row[colIndex] != value) {
						match = false;
					}
					};
				matchLambda(match);

				++colIndex;
			}

			if (match) {
				it = rows.erase(it);
				found = true;
			}
			else {
				++it;
			}
		}

		if (found) {
			saveDataToFile(dataFile, rows);
		}
		else {
			std::cerr << "Error: row not found for deletion.\n";
		}
	}

	bool Table::writeRowToFile(const std::vector<std::string>& row) const {
		std::ofstream file(dataFile, std::ios::app);
		if (!file.is_open()) {
			std::cerr << "Error opening file: " << dataFile << std::endl;
			return false;
		}

		for (const auto& data : row) {
			file << data << static_cast<char>(MiniDb::SEP);
		}
		file << '\n';

		file.close();
		return true;
	}

	bool Table::saveDataToFile(const std::string& filename, const std::vector<std::vector<std::string>>& rows) const {
		std::ofstream file(filename, std::ios::trunc);
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
		for (const auto& column : metadata.columns) {
			std::cout << column.name << "\t";
		}
		std::cout << "\n";
		selectAll();
	}

}
