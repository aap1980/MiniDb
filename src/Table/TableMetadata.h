#pragma once
#include <vector>
#include <string>
#include "Column.h"

namespace MiniDb::Table {

	class TableMetadata {
	private:
		std::string tableName;
		std::string filename;
	public:
		TableMetadata() = default;
		TableMetadata(const std::string tableName);
		std::vector<MiniDb::Table::Column> columns;
		void loadFromFile();
		void saveToFile();
		void addColumn(const MiniDb::Table::Column column);
		void addColumns(const std::vector<Column>& newColumns);
	};

}
