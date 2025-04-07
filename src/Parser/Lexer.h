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
		char peekChar() const;
		void incrementPosition();
		const std::string& statement;
		size_t currentPosition;
	public:
		explicit Lexer(const std::string& input);
		Token nextToken();
		std::vector<Token> getAllTokens();
	};

}