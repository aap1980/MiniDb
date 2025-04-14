#pragma once

#include "Row.h"
#include <vector>

namespace MiniDb::Table {

	class Rows {
	public:
		std::size_t size() const;
		void addRow(const Row& row);
		const std::vector<Row>& getRow() const;

	private:
		std::vector<Row> _rows;
	};

}
