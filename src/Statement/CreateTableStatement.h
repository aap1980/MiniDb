#pragma once

#include "Statement.h"
#include "../SqlParser/SQLParser.h"
#include "../Table/QueryResult.h"
#include <memory>
#include <string>

namespace MiniDb::Statement {

	class CreateTableStatement : public Statement {
	public:
		explicit CreateTableStatement(std::unique_ptr<hsql::SQLParserResult> parserResult);
		bool returnsResult() const override;
		void executeNoResult(MiniDb::Database::Database& database) const override;

	private:
		std::unique_ptr<hsql::SQLParserResult> _parserResult = nullptr;
		const hsql::CreateStatement* _statement = nullptr;
	};

}