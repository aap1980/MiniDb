#pragma once
#include <string>
#include <vector>
#include <optional>

// Abstract Syntax Tree
namespace MiniDb::Parser {

	// Prosta struktura dla klauzuli WHERE (na razie tylko 'kolumna = warto��')
	struct SimpleWhereClause {
		std::string columnName;
		std::string op; // Przechowujemy jako string, np. "="
		std::string literalValue; // Przechowujemy jako string
	};

	// Struktura reprezentuj�ca sparsowane zapytanie SELECT
	struct SelectStatement {
		std::vector<std::string> columns; // Lista nazw kolumn
		std::string fromTable;            // Nazwa tabeli
		std::optional<SimpleWhereClause> whereCondition; // Opcjonalna klauzula WHERE

		// Pomocnicze do wypisywania
		void print() const;
	};

	// W przysz�o�ci mo�na doda� inne typy zapyta�:
	// struct CreateTableStatement { /* ... */ };
	// using ParsedStatement = std::variant<SelectStatement, CreateTableStatement>;

}