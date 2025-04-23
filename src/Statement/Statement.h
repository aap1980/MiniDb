#pragma once

#include <memory>
#include <string>
#include "../SqlParser/SQLParserResult.h"
#include "../Database/Database.h"
#include "../Table/QueryResult.h"

namespace MiniDb::Statement {

	class Statement {
	public:
		virtual ~Statement() = default;

		// Czy to zapytanie zwracające zestaw wierszy?
		virtual bool returnsResult() const = 0;

		// Wykonuje zapytania: CREATE, DELET
		virtual void executeNoResult(MiniDb::Database::Database& database) const {
			throw std::logic_error("executeNoResult() not supported for this statement");
		}

		// Wykonuje zapytania: SELECT
		virtual std::unique_ptr<MiniDb::Table::QueryResult> executeResult(MiniDb::Database::Database& database) const {
			throw std::logic_error("executeResult() not supported for this statement");
		}

		// Fabryka: tworzy Statement z wyniku SQLParsera
		static std::unique_ptr<Statement> fromSQL(const std::string& sql);
	};

}
