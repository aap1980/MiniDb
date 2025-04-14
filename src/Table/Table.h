#pragma once

#include <vector>
#include <string>
#include "Rows.h"
#include "Columns.h"
#include "QueryCondition.h"
#include "TableMetadata.h"
#include "ColumnUpdate.h"

namespace MiniDb::Table {

	class Table {
	private:
		std::string _tableName;
		std::string _dataFile;
		bool writeRowToFile(const std::vector<std::string>& row) const;
		bool readDataFromFile(const std::string& _filename, Rows& rows) const;
	public:
		Columns columns;
		Rows rows;
		Table(const std::string& _tableName);
		MiniDb::Table::TableMetadata metadata;
		void saveToFile();
		void addRow(const std::vector<std::string>& row);
		//void updateRow(const QueryCondition& condition, const std::vector<MiniDb::Table::ColumnUpdate>& columns);
		//void deleteRow(const QueryCondition& condition);
		void printTable() const;
		void selectAll() const;
	};

}
