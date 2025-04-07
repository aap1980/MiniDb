#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <algorithm>

namespace MiniDb::Parser {

	Lexer::Lexer(const std::string& input) : statement(input), currentPosition(0) {}

	Token Lexer::nextToken() {
		skipWhitespace();

		if (currentPosition >= statement.length()) {
			return Token(TokenType::END_OF_FILE, "");
		}

		char currentChar = peekChar();

		// Identyfikatory i s³owa kluczowe
		if (isalpha(currentChar) || currentChar == '_') {
			return readIdentifierOrKeyword();
		}

		// Liczby ca³kowite
		if (isdigit(currentChar)) {
			return readIntegerLiteral();
		}

		// Operatory i separatory
		switch (currentChar) {
		case ',':
			incrementPosition();
			return Token(TokenType::COMMA, ",");
		case '=':
			incrementPosition();
			return Token(TokenType::EQUAL_OPERATOR, "=");
		case ';':
			incrementPosition();
			return Token(TokenType::SEMICOLON, ";");
			// Mo¿na dodaæ obs³ugê innych znaków jak '(', ')', '*', '<', '>' itd.
		}

		// Jeœli nic nie pasuje
		incrementPosition();
		return Token(TokenType::UNKNOWN, std::string(1, currentChar));
	}

	std::vector<Token> Lexer::getAllTokens() {
		std::vector<Token> tokens;
		Token token;
		do {
			token = nextToken();
			tokens.push_back(token);
		} while (token.type != TokenType::END_OF_FILE && token.type != TokenType::UNKNOWN);
		return tokens;
	}

	void Lexer::skipWhitespace() {
		while (currentPosition < statement.length() && isspace(peekChar())) {
			incrementPosition();
		}
	}

	Token Lexer::readIdentifierOrKeyword() {
		size_t startPos = currentPosition;
		while (currentPosition < statement.length() && (isalnum(peekChar()) || peekChar() == '_')) {
			incrementPosition();
		}
		std::string value = statement.substr(startPos, currentPosition - startPos);

		// SprawdŸ, czy to s³owo kluczowe (ignoruj¹c wielkoœæ liter)
		std::string lowerValue = value;
		std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);

		if (lowerValue == "select") return Token(TokenType::SELECT_KEYWORD, value);
		if (lowerValue == "from")   return Token(TokenType::FROM_KEYWORD, value);
		if (lowerValue == "where")  return Token(TokenType::WHERE_KEYWORD, value);

		return Token(TokenType::IDENTIFIER, value);
	}

	Token Lexer::readIntegerLiteral() {
		size_t startPos = currentPosition;
		while (currentPosition < statement.length() && isdigit(peekChar())) {
			incrementPosition();
		}
		std::string value = statement.substr(startPos, currentPosition - startPos);
		return Token(TokenType::INTEGER_LITERAL, value);
	}

	char Lexer::peekChar() const {
		if (currentPosition >= statement.length()) {
			return '\0'; // Koniec pliku
		}
		return statement[currentPosition];
	}

	void Lexer::incrementPosition() {
		if (currentPosition < statement.length()) {
			currentPosition++;
		}
	}

}