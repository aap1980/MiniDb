#pragma once
#include "Token.h"
#include <string>
#include <vector>

namespace MiniDb::Parser {

	class Lexer {
	private:
		void skipWhitespace();
		Token readIdentifierOrKeyword();
		Token readIntegerLiteral();

		// Zwraca bie��cy znak lub '\0' je�li koniec pliku
		char peek() const;

		// Przesuwa pozycj� o jeden znak
		void advance();

		const std::string& m_input; // Referencja do oryginalnego ci�gu SQL
		size_t m_position;          // Bie��ca pozycja w ci�gu m_input
	public:
		explicit Lexer(const std::string& input);
		Token nextToken();
		std::vector<Token> getAllTokens();
	};

}