#include "Statement.h"
#include "CreateTableStatement.h"
#include "SelectStatement.h"
#include "../SqlParser/SQLParser.h"
#include "../SqlParser/SQLParserResult.h"
#include "../SqlParser/util/sqlhelper.h"
#include <iostream>

namespace MiniDb::Statement {

	std::unique_ptr<Statement> Statement::fromSQL(const std::string& sql) {
		auto result = std::make_unique<hsql::SQLParserResult>();
		hsql::SQLParser::parse(sql, result.get());

		if (!result->isValid() || result->size() == 0) {
			std::cerr << "SQL parsing error: " << result->errorMsg() << std::endl;
			return nullptr;
		}

		const hsql::SQLStatement* stmt = result->getStatement(0);
		switch (stmt->type()) {
		case hsql::kStmtCreate:
			return std::make_unique<CreateTableStatement>(std::move(result));
		case hsql::kStmtSelect:
			return std::make_unique<SelectStatement>(std::move(result));
		default:
			std::cerr << "Unsupported query type." << std::endl;
			return nullptr;
		}

	}

}
