#pragma once

#include <string>
#include "../Table/Column.h"

namespace MiniDb::Statement {

	// Informacje o kolumnie wybranej w SELECT
	class SelectedColumnInfo {
	public:
		SelectedColumnInfo(const std::string& tableAlias, const std::string& columnName, const MiniDb::Table::Column& column);
		std::string tableAlias;
		std::string columnName;
		const MiniDb::Table::Column& column;
	};

}
