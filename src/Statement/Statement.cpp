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

		const hsql::SQLStatement* stmt = result.getStatement(0);
		switch (stmt->type()) {
		case hsql::kStmtSelect:
			return std::make_unique<SelectStatement>(
				static_cast<const hsql::SelectStatement*>(stmt));
		default:
			std::cerr << "Nieobs³ugiwany typ zapytania." << std::endl;
			return nullptr;
		}
	}

}
