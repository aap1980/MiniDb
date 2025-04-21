#include "QueryTableInfo.h"

namespace MiniDb::Statement {

	QueryTableInfo::QueryTableInfo(const std::string& alias,
		const std::string& tableName,
		MiniDb::Table::Table& table)
		: alias(alias),
		tableName(tableName),
		table(table) {
	}

}
