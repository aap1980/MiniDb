#pragma once

#include <vector>
#include <string>
#include "Rows.h"
#include "Columns.h"
#include "QueryCondition.h"
#include "ColumnUpdate.h"

namespace MiniDb::Table {

	class Table {
	private:
		std::string _tableName;
		std::string _dataFile;
		std::string _metadataFile;

	public:
		Columns columns;
		Rows rows;
		Table(const std::string& tableName);
		//void addRow(const std::vector<std::string>& row);
		//void updateRow(const QueryCondition& condition, const std::vector<MiniDb::Table::ColumnUpdate>& columns);
		//void deleteRow(const QueryCondition& condition);
		void loadMetadataFromFile();
		void saveMetadataToFile();
		void loadDataFromFile();
		void saveDataToFile();
		void printTable() const;
	};

}
