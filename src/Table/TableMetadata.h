#pragma once
#include <vector>
#include <string>
#include "Column.h"

namespace MiniDb::Table {

	class TableMetadata {
	private:
		std::string tableName;
		std::string filename;
		void loadFromFile();
		void saveToFile();
	public:
		TableMetadata(const std::string tableName);
		std::vector<MiniDb::Table::Column> columns;
		void addColumn(const MiniDb::Table::Column column);
	};

}
