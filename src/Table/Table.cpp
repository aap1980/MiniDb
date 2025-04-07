#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "Table.h"
#include "../Constants.h"
#include "../Config/Config.h"
#include "../ErrorHandling.h"
#include "TableMetadata.h"
#include "ColumnUpdate.h"

namespace MiniDb::Table {

	Table::Table(const std::string& tableName)
		: tableName(tableName), metadata(tableName) {
		std::string tablesPath = MiniDb::Config::Config::getInstance().getTablesPath();
		dataFile = tablesPath + tableName + ".dat";
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

	void Table::saveToFile() {
		std::ofstream file(dataFile);
		if (!file.is_open()) {
			throw FileWriteException("Error opening data file: " + dataFile);
		}

		file.close();
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

	void Table::updateRow(const QueryCondition& condition, const std::vector<MiniDb::Table::ColumnUpdate>& columns) {
		std::vector<std::vector<std::string>> rows;
		if (!readDataFromFile(dataFile, rows)) {
			return;
		}

		bool found = false;
		for (auto& row : rows) {
			bool match = true;

			const auto& conditions = condition.getConditions();
			size_t colIndex = 0;

			// Sprawdzanie, czy warunki w QueryCondition pasuj¹ do wiersza
			for (const auto& [column, value] : conditions) {
				auto matchLambda = [&row, &column, &value, &colIndex, this](bool& match) {
					if (metadata.columns[colIndex].name == column && row[colIndex] != value) {
						match = false;
					}
					};
				matchLambda(match);

				++colIndex;
			}

			// Aktualizacja odpowiednich kolumn na podstawie ColumnUpdate
			if (match) {

				for (const auto& columnUpdate : columns) {
					const std::string& columnName = columnUpdate.getName();
					const std::string& newValue = columnUpdate.getValue();

					for (size_t i = 0; i < metadata.columns.size(); ++i) {
						if (metadata.columns[i].name == columnName) {
							row[i] = newValue;
							break;
						}
					}
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

	void Table::saveDataToFile(const std::string& filename, const std::vector<std::vector<std::string>>& rows) const {
		std::ofstream file(filename, std::ios::trunc);
		if (!file.is_open()) {
			throw FileWriteException("Error opening file: " + filename);
		}

		try {
			for (const auto& row : rows) {
				for (const auto& data : row) {
					file << data << static_cast<char>(MiniDb::SEP);
				}
				file << '\n';
			}
		}
		catch (const std::exception& e) {
			file.close();
			throw FileWriteException("Error writing to file: " + filename + " - " + e.what());
		}

		file.close();
	}

	std::string shortenText(const std::string& text) {
		if (text.length() > TEXT_DISPLAY_LIMIT) {
			return text.substr(0, TEXT_DISPLAY_LIMIT) + "..";
		}
		return text;
	}

	void Table::selectAll() const {
		std::ifstream file(dataFile);
		if (!file.is_open()) {
			std::cerr << "Error opening data file: " << dataFile << std::endl;
			return;
		}

		std::vector<std::vector<std::string>> rows;
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

		for (const auto& row : rows) {
			for (const auto& data : row) {
				std::cout << std::setw(TEXT_DISPLAY_LIMIT) << shortenText(data) << "  ";
			}
			std::cout << "\n";
		}

		file.close();
	}

	void Table::printTable() const {
		std::cout << "Tabela: " << tableName << "\n";

		for (const auto& column : metadata.columns) {
			std::cout << std::setw(TEXT_DISPLAY_LIMIT) << shortenText(column.name) << "  ";
		}
		std::cout << "\n";

		selectAll();
	}

}
