#pragma once

#include "Statement.h"
#include "../Database/Database.h"
#include "../SqlParser/SQLParser.h"
#include "../Table/QueryResult.h"

namespace MiniDb::Statement {

	class SelectStatement : public Statement {
	public:
		explicit SelectStatement(std::unique_ptr<hsql::SQLParserResult> parserResult);
		std::unique_ptr<MiniDb::Table::QueryResult> execute(MiniDb::Database::Database& database) const override;

	private:
		std::unique_ptr<hsql::SQLParserResult> _parserResult = nullptr;
		const hsql::SelectStatement* _statement = nullptr;
	};

}
