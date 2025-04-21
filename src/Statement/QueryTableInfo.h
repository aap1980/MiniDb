#pragma once

#include <string>
#include <optional>
#include "ParsedJoinCondition.h"
#include "../Table/Table.h"

namespace MiniDb::Statement {

	// Informacje o tabeli bior¹cej udzia³ w zapytaniu
	class QueryTableInfo {
	public:
		QueryTableInfo(const std::string& alias, const std::string& tableName, MiniDb::Table::Table& table);
		std::string alias;
		std::string tableName;
		MiniDb::Table::Table table;
		std::optional<ParsedJoinCondition> joinCondition;
	};

}
