#include "QueryTableInfo.h"

namespace MiniDb::Statement {

	QueryTableInfo::QueryTableInfo(const std::string& alias,
		const std::string& tableName,
		Table::Table table)
		: _alias(alias),
		_tableName(tableName),
		_table(std::move(table)) {
	}

	const std::string& QueryTableInfo::alias() const {
		return _alias;
	}

	const std::string& QueryTableInfo::tableName() const {
		return _tableName;
	}

	const Table::Table& QueryTableInfo::table() const {
		return _table;
	}

	const std::optional<ParsedJoinCondition>& QueryTableInfo::joinCondition() const {
		return _joinCondition;
	}

	void QueryTableInfo::setJoinCondition(ParsedJoinCondition condition) {
		_joinCondition = std::move(condition);
	}

}
