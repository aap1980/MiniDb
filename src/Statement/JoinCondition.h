#pragma once

#include <string>
#include "../SqlParser/SQLParserResult.h" 

namespace MiniDb::Statement {

	class JoinCondition {
	public:
		JoinCondition(const std::string& leftTableAlias,
			const std::string& leftColumnName,
			const std::string& rightTableAlias,
			const std::string& rightColumnName,
			hsql::OperatorType opType = hsql::kOpEquals);

		std::string leftTableAlias;
		std::string leftColumnName;
		std::string rightTableAlias;
		std::string rightColumnName;
		hsql::OperatorType opType;
	};

}
