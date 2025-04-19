#pragma once

#include <string>
#include <optional>
#include "ParsedJoinCondition.h"
#include "../Table/Table.h"

namespace MiniDb::Statement {

	class QueryTableInfo {
	public:
		QueryTableInfo(const std::string& alias,
			const std::string& tableName,
			Table::Table table);

		const std::string& alias() const;
		const std::string& tableName() const;
		const Table::Table& table() const;

		const std::optional<ParsedJoinCondition>& joinCondition() const;
		void setJoinCondition(ParsedJoinCondition condition);

	private:
		std::string _alias;
		std::string _tableName;
		Table::Table _table;
		std::optional<ParsedJoinCondition> _joinCondition;
	};

}
