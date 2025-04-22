#include "JoinCondition.h"

namespace MiniDb::Statement {

	JoinCondition::JoinCondition(const std::string& leftTableAlias,
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
