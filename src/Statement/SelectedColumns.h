#pragma once

#include "SelectedColumns.h"
#include "SelectedColumn.h"
#include <vector>

namespace MiniDb::Statement {

	class SelectedColumns {
	public:
		std::vector<MiniDb::Statement::SelectedColumn> columns;
		std::size_t size() const;
		void addColumn(const MiniDb::Statement::SelectedColumn& selectedColumnInfo);
	};

}
