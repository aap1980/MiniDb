#include "SelectedColumn.h"
#include "../Table/Column.h"

namespace MiniDb::Statement {

	SelectedColumn::SelectedColumn(const std::string& tableAlias,
		const std::string& columnName,
		const MiniDb::Table::Column& column)
		: tableAlias(tableAlias),
		columnName(columnName),
		column(column) {
	}

}
