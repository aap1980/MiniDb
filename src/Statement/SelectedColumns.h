#pragma once

#include "SelectedColumns.h"
#include "SelectedColumnInfo.h"
#include <vector>

namespace MiniDb::Statement {

	class SelectedColumns {
	public:
		std::vector<MiniDb::Statement::SelectedColumnInfo> columns;
		std::size_t size() const;
		void addColumn(const MiniDb::Statement::SelectedColumnInfo& selectedColumnInfo);
	};

}
