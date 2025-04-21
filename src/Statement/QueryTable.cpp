#include "QueryTable.h"

namespace MiniDb::Statement {

	QueryTable::QueryTable(const std::string& tableAlias,
		const std::string& tableName,
		MiniDb::Table::Table& table)
		: tableAlias(tableAlias),
		tableName(tableName),
		table(table) {
	}

}
