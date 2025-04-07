#pragma once
#include <vector>
#include <string>
#include "Column.h"
#include "QueryCondition.h"
#include "TableMetadata.h"

namespace MiniDb::Table {

	class Table {
	private:
		std::string tableName;
		std::string dataFile;
		bool loadData(const std::string& dataFile);
		bool writeRowToFile(const std::vector<std::string>& row) const;
		bool saveDataToFile(const std::string& filename, const std::vector<std::vector<std::string>>& rows) const;
		bool readDataFromFile(const std::string& filename, std::vector<std::vector<std::string>>& rows) const;
	public:
		Table(const std::string& tableName);
		MiniDb::Table::TableMetadata metadata;
		void addRow(const std::vector<std::string>& row);
		void updateRow(const QueryCondition& condition, const std::vector<std::string>& newRow);
		void deleteRow(const QueryCondition& condition);
		void printTable() const;
		void selectAll() const;
	};

}
