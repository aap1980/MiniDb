#include "ParsedJoinCondition.h"

namespace MiniDb::Statement {

	ParsedJoinCondition::ParsedJoinCondition(const std::string& leftTableAlias,
		const std::string& leftColumnName,
		const std::string& rightTableAlias,
		const std::string& rightColumnName,
		hsql::OperatorType opType)
		: leftTableAlias(leftTableAlias),
		leftColumnName(leftColumnName),
		rightTableAlias(rightTableAlias),
		rightColumnName(rightColumnName),
		opType(opType) {
	}

}
