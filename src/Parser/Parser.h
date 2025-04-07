#pragma once
#include "Lexer.h"
#include "AST.h" 
#include <memory>
#include <stdexcept>

namespace MiniDb::Parser {

	class Parser {
	private:
		// Przechowuje referencj� do leksera dostarczaj�cego tokeny
		Lexer& lexer;
		// Przechowuje bie��cy token pobrany z leksera
		Token currentToken;

		// Pobiera kolejny token z leksera
		void nextToken();

		// Sprawdza, czy bie��cy token ma oczekiwany typ.
		// Je�li tak, pobiera kolejny token (konsumuje bie��cy).
		// Je�li nie, rzuca wyj�tek.
		void consume(TokenType expectedType);

		// Parsuje list� kolumn
		std::vector<std::string> parseColumnList();

		// Parsuje klauzul� WHERE
		std::unique_ptr<Expression> parseWhereClause();

		// Parsuje ca�e wyra�enie (potencjalnie z AND/OR)
		std::unique_ptr<Expression> parseExpression();
		// Parsuje cz�on wyra�enia po��czony przez AND
		std::unique_ptr<Expression> parseAndTerm();
		// Parsuje podstawowe por�wnanie (np. col = val)
		std::unique_ptr<Expression> parseComparison();
		// Parsuje najbardziej podstawowe elementy (litera�, nazwa kolumny)
		std::unique_ptr<Expression> parsePrimaryExpression();

	public:
		explicit Parser(Lexer& lexer);

		// G��wna funkcja parsowania - pr�buje sparsowa� zapytanie SELECT
		// Zwraca struktur� AST reprezentuj�c� zapytanie
		// Rzuca wyj�tek std::runtime_error w przypadku b��du sk�adni
		SelectStatement parseSelectStatement();
	};

}