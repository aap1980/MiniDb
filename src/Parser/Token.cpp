#include "Token.h"
#include <utility>

namespace MiniDb::Parser {

	std::string tokenTypeToString(TokenType type) {
		switch (type) {
		case TokenType::SELECT_KEYWORD:   return "SELECT_KEYWORD";
		case TokenType::FROM_KEYWORD:     return "FROM_KEYWORD";
		case TokenType::WHERE_KEYWORD:    return "WHERE_KEYWORD";
		case TokenType::IDENTIFIER:       return "IDENTIFIER";
		case TokenType::INTEGER_LITERAL:  return "INTEGER_LITERAL";
		case TokenType::COMMA:            return "COMMA";
		case TokenType::EQUAL_OPERATOR:   return "EQUAL_OPERATOR";
		case TokenType::SEMICOLON:        return "SEMICOLON";
		case TokenType::END_OF_FILE:      return "END_OF_FILE";
		case TokenType::UNKNOWN:          return "UNKNOWN";
		default:                          return "INVALID_TOKEN_TYPE";
		}
	}

	std::ostream& operator<<(std::ostream& os, const Token& token) {
		os << "Token(" << tokenTypeToString(token.type) << ", \"" << token.value << "\")";
		return os;
	}

}