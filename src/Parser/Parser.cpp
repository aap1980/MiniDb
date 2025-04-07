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
			consume(TokenType::WHERE_KEYWORD); // Skonsumuj 'WHERE'
			stmt.whereCondition = parseExpression();
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

	// G³ówna funkcja do parsowania wyra¿enia (na razie obs³uguje tylko AND)
	std::unique_ptr<Expression> Parser::parseExpression() {
		// Na razie wyra¿enie to tylko sekwencja termów po³¹czonych przez AND
		return parseAndTerm();
		// W przysz³oœci, aby obs³u¿yæ OR, to by³oby:
		// auto left = parseAndTerm();
		// while (currentToken.type == TokenType::OR_KEYWORD) {
		//     consume(TokenType::OR_KEYWORD);
		//     auto right = parseAndTerm();
		//     left = std::make_unique<BinaryOperationExpression>(std::move(left), BinaryOperatorType::OR, std::move(right));
		// }
		// return left;
	}

	// Parsuje termy po³¹czone przez AND
	std::unique_ptr<Expression> Parser::parseAndTerm() {
		auto left = parseComparison(); // Zacznij od porównania

		while (currentToken.type == TokenType::AND_KEYWORD) {
			consume(TokenType::AND_KEYWORD);
			auto right = parseComparison(); // Parsuj kolejne porównanie
			// Stwórz wêze³ AND ³¹cz¹cy poprzedni wynik (left) z nowym (right)
			left = std::make_unique<BinaryOperationExpression>(std::move(left), BinaryOperatorType::AND, std::move(right));
		}
		return left; // Zwróæ drzewo (lub pojedyncze porównanie, jeœli nie by³o AND)
	}

	// Parsuje proste porównanie: IDENTIFIER = LITERAL
	std::unique_ptr<Expression> Parser::parseComparison() {
		// Oczekujemy identyfikatora (lewa strona)
		if (currentToken.type != TokenType::IDENTIFIER) {
			throw std::runtime_error("Parser Error: Expected column name (IDENTIFIER) in comparison, got " + tokenTypeToString(currentToken.type));
		}
		auto leftExpr = std::make_unique<ColumnIdentifierExpression>(currentToken.value);
		consume(TokenType::IDENTIFIER);

		// Oczekujemy operatora '=' (na razie tylko ten)
		if (currentToken.type != TokenType::EQUAL_OPERATOR) {
			throw std::runtime_error("Parser Error: Expected EQUAL_OPERATOR (=) after column name, got " + tokenTypeToString(currentToken.type));
		}
		BinaryOperatorType op = BinaryOperatorType::EQUALS; // Mo¿na dodaæ mapowanie z tokena na typ operatora
		consume(TokenType::EQUAL_OPERATOR);

		// Oczekujemy litera³u (prawa strona)
		auto rightExpr = parsePrimaryExpression(); // Litera³y s¹ wyra¿eniami pierwotnymi

		return std::make_unique<BinaryOperationExpression>(std::move(leftExpr), op, std::move(rightExpr));
	}

	// Parsuje podstawowe elementy: litera³y lub nazwy kolumn (choæ tu oczekujemy litera³ów)
	std::unique_ptr<Expression> Parser::parsePrimaryExpression() {
		if (currentToken.type == TokenType::INTEGER_LITERAL) {
			int value = std::stoi(currentToken.value); // Konwersja na int
			consume(TokenType::INTEGER_LITERAL);
			return std::make_unique<LiteralExpression>(value);
		}
		else if (currentToken.type == TokenType::STRING_LITERAL) {
			std::string value = currentToken.value; // Wartoœæ jest ju¿ tekstem
			consume(TokenType::STRING_LITERAL);
			return std::make_unique<LiteralExpression>(std::move(value));
		}
		// Mo¿na by te¿ pozwoliæ na porównanie kolumny z kolumn¹:
		// else if (currentToken.type == TokenType::IDENTIFIER) {
		//     std::string colName = currentToken.value;
		//     consume(TokenType::IDENTIFIER);
		//     return std::make_unique<ColumnIdentifierExpression>(std::move(colName));
		// }
		else {
			throw std::runtime_error("Parser Error: Expected literal (INTEGER or STRING) or column identifier, got " + tokenTypeToString(currentToken.type));
		}
	}

}