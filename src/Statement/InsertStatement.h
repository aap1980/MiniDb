#pragma once

#include "Statement.h"
#include "../SqlParser/SQLParser.h"
#include <memory>
#include <string>

namespace MiniDb::Statement {

	class InsertStatement : public Statement {
	public:
		explicit InsertStatement(std::unique_ptr<hsql::SQLParserResult> parserResult);
		bool returnsResult() const override;
		void executeNoResult(MiniDb::Database::Database& database) const override;

	private:
		std::unique_ptr<hsql::SQLParserResult> _parserResult = nullptr;
		const hsql::InsertStatement* _statement = nullptr;
		std::string valueExprToString(const hsql::Expr* expr) const;
		bool isValueExprNull(const hsql::Expr* expr) const;
	};

}