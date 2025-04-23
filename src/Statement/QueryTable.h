#pragma once

#include <string>
#include <optional>
#include "JoinCondition.h"
#include "../Table/Table.h"

namespace MiniDb::Statement {

	class QueryTable {
	public:
		QueryTable(const std::string& tableAlias, const std::string& tableName, MiniDb::Table::Table& table);
		std::string tableAlias;
		std::string tableName;
		MiniDb::Table::Table table;
		std::optional<JoinCondition> joinCondition;
	};

}
