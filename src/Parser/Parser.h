#pragma once
#include "Lexer.h"
#include "AST.h" 
#include <memory>
#include <stdexcept>

namespace MiniDb::Parser {

	class Parser {
	public:
		explicit Parser(Lexer& lexer);

		// G³ówna funkcja parsowania - próbuje sparsowaæ zapytanie SELECT
		// Zwraca strukturê AST reprezentuj¹c¹ zapytanie
		// Rzuca wyj¹tek std::runtime_error w przypadku b³êdu sk³adni
		SelectStatement parseSelectStatement();

	private:
		// Przechowuje referencjê do leksera dostarczaj¹cego tokeny
		Lexer& lexer;
		// Przechowuje bie¿¹cy token pobrany z leksera
		Token currentToken;

		// Pobiera kolejny token z leksera i zapisuje w m_currentToken
		void nextToken();

		// Sprawdza, czy bie¿¹cy token ma oczekiwany typ.
		// Jeœli tak, pobiera kolejny token (konsumuje bie¿¹cy).
		// Jeœli nie, rzuca wyj¹tek.
		void consume(TokenType expectedType);

		// Parsuje listê kolumn
		std::vector<std::string> parseColumnList();

		// Parsuje klauzulê WHERE
		SimpleWhereClause parseWhereClause();
	};

}