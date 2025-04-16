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

		// G³ówna metoda wykonuj¹ca zapytanie
		virtual std::unique_ptr<MiniDb::Table::QueryResult> execute(MiniDb::Database::Database& database) const = 0;

		// Fabryka: tworzy Statement z wyniku SQLParsera
		static std::unique_ptr<Statement> fromSQL(const std::string& sql);
	};

}
