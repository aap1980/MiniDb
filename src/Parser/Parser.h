#pragma once
#include "Lexer.h"
#include "AST.h" 
#include <memory>
#include <stdexcept>

namespace MiniDb::Parser {

	class Parser {
	public:
		explicit Parser(Lexer& lexer);

		// G��wna funkcja parsowania - pr�buje sparsowa� zapytanie SELECT
		// Zwraca struktur� AST reprezentuj�c� zapytanie
		// Rzuca wyj�tek std::runtime_error w przypadku b��du sk�adni
		SelectStatement parseSelectStatement();

	private:
		// Przechowuje referencj� do leksera dostarczaj�cego tokeny
		Lexer& lexer;
		// Przechowuje bie��cy token pobrany z leksera
		Token currentToken;

		// Pobiera kolejny token z leksera i zapisuje w m_currentToken
		void nextToken();

		// Sprawdza, czy bie��cy token ma oczekiwany typ.
		// Je�li tak, pobiera kolejny token (konsumuje bie��cy).
		// Je�li nie, rzuca wyj�tek.
		void consume(TokenType expectedType);

		// Parsuje list� kolumn
		std::vector<std::string> parseColumnList();

		// Parsuje klauzul� WHERE
		SimpleWhereClause parseWhereClause();
	};

}