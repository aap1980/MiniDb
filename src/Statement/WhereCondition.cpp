#include "WhereCondition.h"

namespace MiniDb::Statement {

	WhereCondition::WhereCondition(std::string tableAlias,
		std::string columnName,
		hsql::OperatorType operatorType,
		LiteralValue literalValue)
		: tableAlias(tableAlias),
		columnName(columnName),
		operatorType(operatorType),
		literalValue(literalValue) {
	}

}
