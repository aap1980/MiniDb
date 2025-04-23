#pragma once

#include "Statement.h"
#include "../SqlParser/SQLParser.h"
#include "../Table/QueryResult.h"
#include <memory>
#include <string>

namespace MiniDb::Statement {

	class CreateStatement : public Statement {
	public:
		explicit CreateStatement(std::unique_ptr<hsql::SQLParserResult> parserResult);
		std::unique_ptr<MiniDb::Table::QueryResult> execute(MiniDb::Database::Database& database) const override;

	private:
		std::unique_ptr<hsql::SQLParserResult> _parserResult = nullptr;
		const hsql::CreateStatement* _statement = nullptr;
	};

}