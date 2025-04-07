#pragma once
#include "Lexer.h"
#include "AST.h" 
#include <memory>
#include <stdexcept>

namespace MiniDb::Parser {

	class Parser {
	private:
		// Przechowuje referencjê do leksera dostarczaj¹cego tokeny
		Lexer& lexer;
		// Przechowuje bie¿¹cy token pobrany z leksera
		Token currentToken;

		// Pobiera kolejny token z leksera
		void nextToken();

		// Sprawdza, czy bie¿¹cy token ma oczekiwany typ.
		// Jeœli tak, pobiera kolejny token (konsumuje bie¿¹cy).
		// Jeœli nie, rzuca wyj¹tek.
		void consume(TokenType expectedType);

		// Parsuje listê kolumn
		std::vector<std::string> parseColumnList();

		// Parsuje klauzulê WHERE
		std::unique_ptr<Expression> parseWhereClause();

		// Parsuje ca³e wyra¿enie (potencjalnie z AND/OR)
		std::unique_ptr<Expression> parseExpression();
		// Parsuje cz³on wyra¿enia po³¹czony przez AND
		std::unique_ptr<Expression> parseAndTerm();
		// Parsuje podstawowe porównanie (np. col = val)
		std::unique_ptr<Expression> parseComparison();
		// Parsuje najbardziej podstawowe elementy (litera³, nazwa kolumny)
		std::unique_ptr<Expression> parsePrimaryExpression();

	public:
		explicit Parser(Lexer& lexer);

		// G³ówna funkcja parsowania - próbuje sparsowaæ zapytanie SELECT
		// Zwraca strukturê AST reprezentuj¹c¹ zapytanie
		// Rzuca wyj¹tek std::runtime_error w przypadku b³êdu sk³adni
		SelectStatement parseSelectStatement();
	};

}