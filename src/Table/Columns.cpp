#include "Columns.h"
#include <stdexcept>

namespace MiniDb::Table {

	std::size_t Columns::size() const {
		return _columns.size();
	}

	void Columns::addColumn(const Column& column) {
		_columns.push_back(column);
	}

	void Columns::addColumns(const std::vector<Column>& columns) {
		_columns.insert(_columns.end(), columns.begin(), columns.end());
	}

	const std::vector<Column>& Columns::getColumns() const {
		return _columns;
	}

	const Column& Columns::getColumnByName(const std::string& name) const {
		for (const auto& column : _columns) {
			if (column.name == name) {
				return column;
			}
		}
		throw std::out_of_range("Column not found: " + name);
	}


}