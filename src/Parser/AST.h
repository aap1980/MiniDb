#pragma once
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <variant>

// Abstract Syntax Tree
namespace MiniDb::Parser {

	// Typy operatorów binarnych
	enum class BinaryOperatorType {
		EQUALS, // =
		AND     // AND
		// OR, LESS_THAN, GREATER_THAN, etc.
	};

	// Bazowa struktura/klasa dla wszystkich wyra¿eñ
	struct Expression {
		virtual ~Expression() = default; // Wirtualny destruktor jest wa¿ny!
		virtual void print(std::ostream& os, int indent = 0) const = 0; // Wirtualna metoda do drukowania drzewa
	};

	// Wyra¿enie reprezentuj¹ce litera³ (liczba, tekst)
	struct LiteralExpression : Expression {
		// std::variant pozwala przechowywaæ ró¿ne typy litera³ów
		std::variant<int, std::string> value;

		explicit LiteralExpression(int v) : value(v) {}
		explicit LiteralExpression(std::string v) : value(std::move(v)) {}

		void print(std::ostream& os, int indent = 0) const override;
	};

	// Wyra¿enie reprezentuj¹ce identyfikator kolumny
	struct ColumnIdentifierExpression : Expression {
		std::string columnName;

		explicit ColumnIdentifierExpression(std::string name) : columnName(std::move(name)) {}

		void print(std::ostream& os, int indent = 0) const override;
	};

	// Wyra¿enie reprezentuj¹ce operacjê binarn¹ (np. id = 1, condition1 AND condition2)
	struct BinaryOperationExpression : Expression {
		std::unique_ptr<Expression> left;
		BinaryOperatorType op;
		std::unique_ptr<Expression> right;

		BinaryOperationExpression(std::unique_ptr<Expression> l, BinaryOperatorType o, std::unique_ptr<Expression> r)
			: left(std::move(l)), op(o), right(std::move(r)) {
		}

		void print(std::ostream& os, int indent = 0) const override;
	};

	// Struktura reprezentuj¹ca sparsowane zapytanie SELECT
	struct SelectStatement {
		std::vector<std::string> columns;
		std::string fromTable;
		// Klauzula WHERE jest teraz opcjonalnym wskaŸnikiem na korzeñ drzewa wyra¿eñ
		std::optional<std::unique_ptr<Expression>> whereCondition;

		// Pomocnicze do wypisywania
		void print() const;
	};

	std::string operatorToString(BinaryOperatorType op);

	// W przysz³oœci mo¿na dodaæ inne typy zapytañ:
	// struct CreateTableStatement { /* ... */ };
	// using ParsedStatement = std::variant<SelectStatement, CreateTableStatement>;

}