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

		// Zwraca bie¿¹cy znak lub '\0' jeœli koniec pliku
		char peek() const;

		// Przesuwa pozycjê o jeden znak
		void advance();

		const std::string& m_input; // Referencja do oryginalnego ci¹gu SQL
		size_t m_position;          // Bie¿¹ca pozycja w ci¹gu m_input
	public:
		explicit Lexer(const std::string& input);
		Token nextToken();
		std::vector<Token> getAllTokens();
	};

}