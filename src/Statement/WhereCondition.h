#pragma once

#include <string>
#include <variant>
#include "../SqlParser/SQLParser.h"

namespace MiniDb::Statement {

	using LiteralValue = std::variant<long, double, std::string>;

	class WhereCondition {
	public:
		WhereCondition(std::string tableAlias,
			std::string columnName,
			hsql::OperatorType opType,
			LiteralValue value);

		std::string tableAlias;
		std::string columnName;
		hsql::OperatorType operatorType;
		LiteralValue literalValue;
	};

}
