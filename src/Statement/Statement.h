#pragma once

#include <memory>
#include <string>
#include "../Database/Database.h"

namespace MiniDb::Statement {

	class Statement {
	public:
		virtual ~Statement() = default;

		// G³ówna metoda wykonuj¹ca zapytanie
		virtual void execute(MiniDb::Database::Database& db) const = 0;

		// Fabryka: tworzy Statement z wyniku SQLParsera
		static std::unique_ptr<Statement> fromSQL(const std::string& sql);
	};

}
