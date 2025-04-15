#include "Columns.h"

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

}