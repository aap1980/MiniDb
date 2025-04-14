#pragma once
#include <vector>
#include <string>
#include "Column.h"

namespace MiniDb::Table {

	class TableMetadata {
	private:
		std::string _tableName;
		std::string _filename;
		void loadFromFile();
	public:
		std::vector<MiniDb::Table::Column> columns;
		TableMetadata() = default;
		void initialize(const std::string tableName);
		void addColumn(const MiniDb::Table::Column column);
		void addColumns(const std::vector<Column>& newColumns);
		void saveToFile();
	};

}
