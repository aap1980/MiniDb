#include "ParsedJoinCondition.h"

namespace MiniDb::Statement {

	ParsedJoinCondition::ParsedJoinCondition(const std::string& leftTableAlias,
		const std::string& leftColumnName,
		const std::string& rightTableAlias,
		const std::string& rightColumnName,
		hsql::OperatorType opType)
		: _leftTableAlias(leftTableAlias),
		_leftColumnName(leftColumnName),
		_rightTableAlias(rightTableAlias),
		_rightColumnName(rightColumnName),
		_opType(opType) {
	}

	const std::string& ParsedJoinCondition::leftTableAlias() const {
		return _leftTableAlias;
	}

	const std::string& ParsedJoinCondition::leftColumnName() const {
		return _leftColumnName;
	}

	const std::string& ParsedJoinCondition::rightTableAlias() const {
		return _rightTableAlias;
	}

	const std::string& ParsedJoinCondition::rightColumnName() const {
		return _rightColumnName;
	}

	hsql::OperatorType ParsedJoinCondition::opType() const {
		return _opType;
	}

}
