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

		// Parsuj list� kolumn
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

		// Sprawd�, czy jest opcjonalna klauzula WHERE
		if (currentToken.type == TokenType::WHERE_KEYWORD) {
			consume(TokenType::WHERE_KEYWORD); // Skonsumuj 'WHERE'
			stmt.whereCondition = parseExpression();
		}

		// Oczekujemy ';' na ko�cu (lub EOF)
		if (currentToken.type == TokenType::SEMICOLON) {
			consume(TokenType::SEMICOLON);
		}
		// Akceptujemy te� brak �rednika je�li to koniec wej�cia
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

		// Dop�ki napotykamy przecinki, parsuj kolejne identyfikatory
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

	// G��wna funkcja do parsowania wyra�enia (na razie obs�uguje tylko AND)
	std::unique_ptr<Expression> Parser::parseExpression() {
		// Na razie wyra�enie to tylko sekwencja term�w po��czonych przez AND
		return parseAndTerm();
		// W przysz�o�ci, aby obs�u�y� OR, to by�oby:
		// auto left = parseAndTerm();
		// while (currentToken.type == TokenType::OR_KEYWORD) {
		//     consume(TokenType::OR_KEYWORD);
		//     auto right = parseAndTerm();
		//     left = std::make_unique<BinaryOperationExpression>(std::move(left), BinaryOperatorType::OR, std::move(right));
		// }
		// return left;
	}

	// Parsuje termy po��czone przez AND
	std::unique_ptr<Expression> Parser::parseAndTerm() {
		auto left = parseComparison(); // Zacznij od por�wnania

		while (currentToken.type == TokenType::AND_KEYWORD) {
			consume(TokenType::AND_KEYWORD);
			auto right = parseComparison(); // Parsuj kolejne por�wnanie
			// Stw�rz w�ze� AND ��cz�cy poprzedni wynik (left) z nowym (right)
			left = std::make_unique<BinaryOperationExpression>(std::move(left), BinaryOperatorType::AND, std::move(right));
		}
		return left; // Zwr�� drzewo (lub pojedyncze por�wnanie, je�li nie by�o AND)
	}

	// Parsuje proste por�wnanie: IDENTIFIER = LITERAL
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
		BinaryOperatorType op = BinaryOperatorType::EQUALS; // Mo�na doda� mapowanie z tokena na typ operatora
		consume(TokenType::EQUAL_OPERATOR);

		// Oczekujemy litera�u (prawa strona)
		auto rightExpr = parsePrimaryExpression(); // Litera�y s� wyra�eniami pierwotnymi

		return std::make_unique<BinaryOperationExpression>(std::move(leftExpr), op, std::move(rightExpr));
	}

	// Parsuje podstawowe elementy: litera�y lub nazwy kolumn (cho� tu oczekujemy litera��w)
	std::unique_ptr<Expression> Parser::parsePrimaryExpression() {
		if (currentToken.type == TokenType::INTEGER_LITERAL) {
			int value = std::stoi(currentToken.value); // Konwersja na int
			consume(TokenType::INTEGER_LITERAL);
			return std::make_unique<LiteralExpression>(value);
		}
		else if (currentToken.type == TokenType::STRING_LITERAL) {
			std::string value = currentToken.value; // Warto�� jest ju� tekstem
			consume(TokenType::STRING_LITERAL);
			return std::make_unique<LiteralExpression>(std::move(value));
		}
		// Mo�na by te� pozwoli� na por�wnanie kolumny z kolumn�:
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