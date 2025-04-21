#pragma once

#include <string>
#include "../Table/Column.h"

namespace MiniDb::Statement {

	// Informacje o kolumnie wybranej w SELECT
	class SelectedColumn {
	public:
		SelectedColumn(const std::string& tableAlias, const std::string& columnName, const MiniDb::Table::Column& column);
		std::string tableAlias;
		std::string columnName;
		const MiniDb::Table::Column& column;
	};

}
