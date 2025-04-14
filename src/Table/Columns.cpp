#include "Columns.h"

namespace MiniDb::Table {

	std::size_t Columns::size() const {
		return _columns.size();
	}

	void Columns::addColumn(const Column& column) {
		_columns.push_back(column);
	}

	const std::vector<Column>& Columns::getColumn() const {
		return _columns;
	}

}