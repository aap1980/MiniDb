#pragma once

#include <string>
#include "../SqlParser/SQLParserResult.h" 

namespace MiniDb::Statement {

	class ParsedJoinCondition {
	public:
		ParsedJoinCondition(const std::string& leftTableAlias,
			const std::string& leftColumnName,
			const std::string& rightTableAlias,
			const std::string& rightColumnName,
			hsql::OperatorType opType = hsql::kOpEquals);

		const std::string& leftTableAlias() const;
		const std::string& leftColumnName() const;
		const std::string& rightTableAlias() const;
		const std::string& rightColumnName() const;
		hsql::OperatorType opType() const;

	private:
		std::string _leftTableAlias;
		std::string _leftColumnName;
		std::string _rightTableAlias;
		std::string _rightColumnName;
		hsql::OperatorType _opType;
	};

}
