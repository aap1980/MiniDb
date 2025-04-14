#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "Row.h"
#include "Rows.h"
#include "Table.h"
#include "../Constants.h"
#include "../Config/Config.h"
#include "../ErrorHandling.h"
#include "TableMetadata.h"
#include "ColumnUpdate.h"

namespace MiniDb::Table {

	Table::Table(const std::string& _tableName)
		: _tableName(_tableName), metadata(), rows() {
		std::string tablesPath = MiniDb::Config::Config::getInstance().getTablesPath();
		_dataFile = tablesPath + _tableName + ".dat";
		metadata.initialize(_tableName);
	}

	bool Table::readDataFromFile(const std::string& _filename, Rows& rows) const {
		std::ifstream file(_filename);
		if (!file.is_open()) {
			std::cerr << "Error opening data file: " << _filename << std::endl;
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
			rows.addRow(Row(std::move(row)));
		}

		file.close();
		return true;
	}

	void Table::saveToFile() {
		std::ofstream file(_dataFile);
		if (!file.is_open()) {
			throw FileWriteException("Error opening data file: " + _dataFile);
		}

		for (const auto& row : rows.getRow()) {
			for (const auto& value : row.getValues()) {
				file << value << static_cast<char>(MiniDb::SEP);
			}
			file << '\n';
		}

		file.close();
	}

	void Table::addRow(const std::vector<std::string>& row) {
		if (row.size() != metadata.columns.size()) {
			std::cerr << "Error: the number of data in a row does not match the number of columns.\n";
			return;
		}

		rows.addRow(Row(row));
		if (!writeRowToFile(row)) {
			std::cerr << "Error: failed to add row to file.\n";
		}
	}

	/*void Table::updateRow(const QueryCondition& condition, const std::vector<MiniDb::Table::ColumnUpdate>& columns) {
		std::vector<std::vector<std::string>> rows;
		if (!readDataFromFile(dataFile, rows)) {
			return;
		}

		bool found = false;
		for (auto& row : rows) {
			bool match = true;

			const auto& conditions = condition.getConditions();
			size_t colIndex = 0;

			// Sprawdzanie, czy warunki w QueryCondition pasuj� do wiersza
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
	}*/

	/*void Table::deleteRow(const QueryCondition& condition) {
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
	}*/

	bool Table::writeRowToFile(const std::vector<std::string>& row) const {
		std::ofstream file(_dataFile, std::ios::app);
		if (!file.is_open()) {
			std::cerr << "Error opening file: " << _dataFile << std::endl;
			return false;
		}

		for (const auto& data : row) {
			file << data << static_cast<char>(MiniDb::SEP);
		}
		file << '\n';

		file.close();
		return true;
	}

	std::string shortenText(const std::string& text) {
		if (text.length() > TEXT_DISPLAY_LIMIT) {
			return text.substr(0, TEXT_DISPLAY_LIMIT) + "..";
		}
		return text;
	}

	void Table::selectAll() const {
		std::ifstream file(_dataFile);
		if (!file.is_open()) {
			std::cerr << "Error opening data file: " << _dataFile << std::endl;
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
		std::cout << "Tabela: " << _tableName << "\n";

		for (const auto& column : metadata.columns) {
			std::cout << std::setw(TEXT_DISPLAY_LIMIT) << shortenText(column.name) << "  ";
		}
		std::cout << "\n";

		selectAll();
	}

}
