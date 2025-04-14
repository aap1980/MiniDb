#pragma once

#include "Column.h"
#include <vector>

namespace MiniDb::Table {

	class Columns {
	public:
		std::size_t size() const;
		void addColumn(const Column& column);
		const std::vector<Column>& getColumn() const;

	private:
		std::vector<Column> _columns;
	};

}
