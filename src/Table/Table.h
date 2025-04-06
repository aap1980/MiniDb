#pragma once
#include <vector>
#include <string>
#include "Column.h"

namespace MiniDb::Table {

	class Table {
	private:
		std::string tableName;
		std::string metadataFile;
		std::string dataFile;
		bool loadMetadata(const std::string& metadataFile);
		bool loadData(const std::string& dataFile);
		bool saveDataToFile(const std::string& filename, const std::vector<std::vector<std::string>>& rows) const;

	public:
		std::vector<MiniDb::Table::Column> columns;
		Table(const std::string& tableName);
		void addRow(const std::vector<std::string>& row);
		void printTable() const;
		void selectAll() const;
	};

}
