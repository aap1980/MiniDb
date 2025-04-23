#pragma once

#include "Statement.h"
#include "../Database/Database.h"
#include "../SqlParser/SQLParser.h"
#include "../Table/QueryResult.h"

namespace MiniDb::Statement {

	class SelectStatement : public Statement {
	public:
		explicit SelectStatement(std::unique_ptr<hsql::SQLParserResult> parserResult);
		bool returnsResult() const override;
		std::unique_ptr<MiniDb::Table::QueryResult> executeResult(MiniDb::Database::Database& database) const override;

	private:
		std::unique_ptr<hsql::SQLParserResult> _parserResult = nullptr;
		const hsql::SelectStatement* _statement = nullptr;
	};

}
