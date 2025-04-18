#pragma once

#include "Column.h"
#include <vector>

namespace MiniDb::Table {

	class Columns {
	public:
		std::size_t size() const;
		void addColumn(const Column& column);
		void addColumns(const std::vector<Column>& columns);
		const std::vector<Column>& getColumns() const;
		const Column& getColumnByName(const std::string& name) const;
		const std::size_t getColumnIndexByName(const std::string& name) const;

	private:
		std::vector<Column> _columns;
	};

}
