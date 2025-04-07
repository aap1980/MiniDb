#include "Parser.h"
#include <iostream> 

namespace MiniDb::Parser {

	Parser::Parser(Lexer& lexer) : lexer(lexer) {
		nextToken();
	}

	void Parser::nextToken() {
		currentToken = lexer.nextToken();
	}

	void Parser::consume(TokenType expectedType) {
		if (currentToken.type == expectedType) {
			nextToken();
		}
		else {
			std::string errorMsg = "Parser Error: Expected token " + tokenTypeToString(expectedType) +
				" but got " + tokenTypeToString(currentToken.type) +
				" ('" + currentToken.value + "')";
			throw std::runtime_error(errorMsg);
		}
	}

	SelectStatement Parser::parseSelectStatement() {
		SelectStatement stmt;

		// Oczekujemy 'SELECT'
		consume(TokenType::SELECT_KEYWORD);

		// Parsuj listê kolumn
		stmt.columns = parseColumnList();

		// Oczekujemy 'FROM'
		consume(TokenType::FROM_KEYWORD);

		// Oczekujemy identyfikatora tabeli
		if (currentToken.type != TokenType::IDENTIFIER) {
			throw std::runtime_error("Parser Error: Expected table name (IDENTIFIER) after FROM, but got " +
				tokenTypeToString(currentToken.type));
		}
		stmt.fromTable = currentToken.value;
		consume(TokenType::IDENTIFIER);

		// SprawdŸ, czy jest opcjonalna klauzula WHERE
		if (currentToken.type == TokenType::WHERE_KEYWORD) {
			stmt.whereCondition = parseWhereClause();
		}

		// Oczekujemy ';' na koñcu (lub EOF)
		if (currentToken.type == TokenType::SEMICOLON) {
			consume(TokenType::SEMICOLON);
		}
		// Akceptujemy te¿ brak œrednika jeœli to koniec wejœcia
		else if (currentToken.type != TokenType::END_OF_FILE) {
			throw std::runtime_error("Parser Error: Expected SEMICOLON or END_OF_FILE at the end of statement, but got " +
				tokenTypeToString(currentToken.type));
		}


		return stmt;
	}

	std::vector<std::string> Parser::parseColumnList() {
		std::vector<std::string> columns;

		// Oczekujemy przynajmniej jednego identyfikatora
		if (currentToken.type != TokenType::IDENTIFIER) {
			throw std::runtime_error("Parser Error: Expected column name (IDENTIFIER) after SELECT, but got " +
				tokenTypeToString(currentToken.type));
		}
		columns.push_back(currentToken.value);
		consume(TokenType::IDENTIFIER);

		// Dopóki napotykamy przecinki, parsuj kolejne identyfikatory
		while (currentToken.type == TokenType::COMMA) {
			consume(TokenType::COMMA);
			if (currentToken.type != TokenType::IDENTIFIER) {
				throw std::runtime_error("Parser Error: Expected column name (IDENTIFIER) after comma, but got " +
					tokenTypeToString(currentToken.type));
			}
			columns.push_back(currentToken.value);
			consume(TokenType::IDENTIFIER);
		}

		return columns;
	}

	SimpleWhereClause Parser::parseWhereClause() {
		consume(TokenType::WHERE_KEYWORD);

		SimpleWhereClause condition;

		// Oczekujemy identyfikatora (lewa strona warunku)
		if (currentToken.type != TokenType::IDENTIFIER) {
			throw std::runtime_error("Parser Error: Expected column name (IDENTIFIER) after WHERE, but got " +
				tokenTypeToString(currentToken.type));
		}
		condition.columnName = currentToken.value;
		consume(TokenType::IDENTIFIER);

		// Oczekujemy operatora '=' (na razie tylko ten)
		if (currentToken.type != TokenType::EQUAL_OPERATOR) {
			throw std::runtime_error("Parser Error: Expected EQUAL_OPERATOR (=) in WHERE clause, but got " +
				tokenTypeToString(currentToken.type));
		}
		condition.op = currentToken.value;
		consume(TokenType::EQUAL_OPERATOR);

		// Oczekujemy litera³u (na razie tylko INTEGER)
		if (currentToken.type != TokenType::INTEGER_LITERAL) {
			throw std::runtime_error("Parser Error: Expected INTEGER_LITERAL in WHERE clause after '=', but got " +
				tokenTypeToString(currentToken.type));
		}
		condition.literalValue = currentToken.value;
		consume(TokenType::INTEGER_LITERAL);

		return condition;
	}

}