#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <algorithm>

namespace MiniDb::Parser {

	Lexer::Lexer(const std::string& input) : m_input(input), m_position(0) {}

	Token Lexer::nextToken() {
		skipWhitespace();

		if (m_position >= m_input.length()) {
			return Token(TokenType::END_OF_FILE, "");
		}

		char currentChar = peek();

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
			advance();
			return Token(TokenType::COMMA, ",");
		case '=':
			advance();
			return Token(TokenType::EQUAL_OPERATOR, "=");
		case ';':
			advance();
			return Token(TokenType::SEMICOLON, ";");
			// Mo¿na dodaæ obs³ugê innych znaków jak '(', ')', '*', '<', '>' itd.
		}

		// Jeœli nic nie pasuje
		advance();
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
		while (m_position < m_input.length() && isspace(peek())) {
			advance();
		}
	}

	Token Lexer::readIdentifierOrKeyword() {
		size_t startPos = m_position;
		while (m_position < m_input.length() && (isalnum(peek()) || peek() == '_')) {
			advance();
		}
		std::string value = m_input.substr(startPos, m_position - startPos);

		// SprawdŸ, czy to s³owo kluczowe (ignoruj¹c wielkoœæ liter)
		std::string lowerValue = value;
		std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);

		if (lowerValue == "select") return Token(TokenType::SELECT_KEYWORD, value);
		if (lowerValue == "from")   return Token(TokenType::FROM_KEYWORD, value);
		if (lowerValue == "where")  return Token(TokenType::WHERE_KEYWORD, value);

		return Token(TokenType::IDENTIFIER, value);
	}

	Token Lexer::readIntegerLiteral() {
		size_t startPos = m_position;
		while (m_position < m_input.length() && isdigit(peek())) {
			advance();
		}
		std::string value = m_input.substr(startPos, m_position - startPos);
		return Token(TokenType::INTEGER_LITERAL, value);
	}

	char Lexer::peek() const {
		if (m_position >= m_input.length()) {
			return '\0'; // Koniec pliku
		}
		return m_input[m_position];
	}

	void Lexer::advance() {
		if (m_position < m_input.length()) {
			m_position++;
		}
	}

}