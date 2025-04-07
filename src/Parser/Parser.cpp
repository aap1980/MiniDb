#include "Parser.h"
#include <iostream> 

namespace MiniDb::Parser {

	Parser::Parser(Lexer& lexer) : m_lexer(lexer) {
		// Pobierz pierwszy token od razu
		nextToken();
	}

	void Parser::nextToken() {
		m_currentToken = m_lexer.nextToken();
	}

	void Parser::consume(TokenType expectedType) {
		if (m_currentToken.type == expectedType) {
			nextToken(); // Pobierz nastêpny token
		}
		else {
			std::string errorMsg = "Parser Error: Expected token " + tokenTypeToString(expectedType) +
				" but got " + tokenTypeToString(m_currentToken.type) +
				" ('" + m_currentToken.value + "')";
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
		if (m_currentToken.type != TokenType::IDENTIFIER) {
			throw std::runtime_error("Parser Error: Expected table name (IDENTIFIER) after FROM, but got " +
				tokenTypeToString(m_currentToken.type));
		}
		stmt.fromTable = m_currentToken.value;
		consume(TokenType::IDENTIFIER); // Skonsumuj nazwê tabeli

		// SprawdŸ, czy jest opcjonalna klauzula WHERE
		if (m_currentToken.type == TokenType::WHERE_KEYWORD) {
			stmt.whereCondition = parseWhereClause();
		}

		// Oczekujemy ';' na koñcu (lub EOF)
		if (m_currentToken.type == TokenType::SEMICOLON) {
			consume(TokenType::SEMICOLON);
		}
		// Akceptujemy te¿ brak œrednika jeœli to koniec wejœcia
		else if (m_currentToken.type != TokenType::END_OF_FILE) {
			throw std::runtime_error("Parser Error: Expected SEMICOLON or END_OF_FILE at the end of statement, but got " +
				tokenTypeToString(m_currentToken.type));
		}


		return stmt;
	}

	std::vector<std::string> Parser::parseColumnList() {
		std::vector<std::string> columns;

		// Oczekujemy przynajmniej jednego identyfikatora
		if (m_currentToken.type != TokenType::IDENTIFIER) {
			throw std::runtime_error("Parser Error: Expected column name (IDENTIFIER) after SELECT, but got " +
				tokenTypeToString(m_currentToken.type));
		}
		columns.push_back(m_currentToken.value);
		consume(TokenType::IDENTIFIER);

		// Dopóki napotykamy przecinki, parsuj kolejne identyfikatory
		while (m_currentToken.type == TokenType::COMMA) {
			consume(TokenType::COMMA); // Skonsumuj przecinek
			if (m_currentToken.type != TokenType::IDENTIFIER) {
				throw std::runtime_error("Parser Error: Expected column name (IDENTIFIER) after comma, but got " +
					tokenTypeToString(m_currentToken.type));
			}
			columns.push_back(m_currentToken.value);
			consume(TokenType::IDENTIFIER);
		}

		return columns;
	}

	SimpleWhereClause Parser::parseWhereClause() {
		consume(TokenType::WHERE_KEYWORD); // Skonsumuj 'WHERE'

		SimpleWhereClause condition;

		// Oczekujemy identyfikatora (lewa strona warunku)
		if (m_currentToken.type != TokenType::IDENTIFIER) {
			throw std::runtime_error("Parser Error: Expected column name (IDENTIFIER) after WHERE, but got " +
				tokenTypeToString(m_currentToken.type));
		}
		condition.columnName = m_currentToken.value;
		consume(TokenType::IDENTIFIER);

		// Oczekujemy operatora '=' (na razie tylko ten)
		if (m_currentToken.type != TokenType::EQUAL_OPERATOR) {
			throw std::runtime_error("Parser Error: Expected EQUAL_OPERATOR (=) in WHERE clause, but got " +
				tokenTypeToString(m_currentToken.type));
		}
		condition.op = m_currentToken.value;
		consume(TokenType::EQUAL_OPERATOR);

		// Oczekujemy litera³u (na razie tylko INTEGER)
		if (m_currentToken.type != TokenType::INTEGER_LITERAL) {
			throw std::runtime_error("Parser Error: Expected INTEGER_LITERAL in WHERE clause after '=', but got " +
				tokenTypeToString(m_currentToken.type));
		}
		condition.literalValue = m_currentToken.value;
		consume(TokenType::INTEGER_LITERAL);

		return condition;
	}

	// Implementacja metody pomocniczej print dla AST
#include <iostream>
	void SelectStatement::print() const {
		std::cout << "SELECT ";
		for (size_t i = 0; i < columns.size(); ++i) {
			std::cout << columns[i] << (i == columns.size() - 1 ? "" : ", ");
		}
		std::cout << "\nFROM " << fromTable;
		if (whereCondition) {
			std::cout << "\nWHERE " << whereCondition->columnName << " "
				<< whereCondition->op << " " << whereCondition->literalValue;
		}
		std::cout << ";" << std::endl;
	}

}