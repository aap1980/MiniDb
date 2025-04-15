#include "Statement.h"
#include "SelectStatement.h"
#include "../SqlParser/SQLParser.h"
#include "../SqlParser/SQLParserResult.h"
#include <iostream>

namespace MiniDb::Statement {

	std::unique_ptr<Statement> Statement::fromSQL(const std::string& sql) {
		hsql::SQLParserResult result;
		hsql::SQLParser::parse(sql, &result);

		if (!result.isValid() || result.size() == 0) {
			std::cerr << "SQL parsing error: " << result.errorMsg() << std::endl;
			return nullptr;
		}

		const hsql::SQLStatement* statement = result.getStatement(0);
		switch (statement->type()) {
		case hsql::kStmtSelect:
			return std::make_unique<SelectStatement>(
				static_cast<const hsql::SelectStatement*>(statement));
		default:
			std::cerr << "Unsupported query type." << std::endl;
			return nullptr;
		}
	}

}
