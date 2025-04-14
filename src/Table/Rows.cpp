#include "Rows.h"

namespace MiniDb::Table {

	std::size_t Rows::size() const {
		return _rows.size();
	}

	void Rows::addRow(const Row& row) {
		_rows.push_back(row);
	}

	const std::vector<Row>& Rows::getRow() const {
		return _rows;
	}

}
