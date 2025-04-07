#include "Parser.h"
#include <stdexcept>

namespace MiniDb::Parser {

	Parser::Parser(const std::string& query) : query(query), currentIndex(0) {}

	ParseNode Parser::parse() {
		return parseSelect();
	}

	ParseNode Parser::parseSelect() {
		skipWhitespace();
		if (query.substr(currentIndex, 6) == "SELECT") {
			currentIndex += 6;
			skipWhitespace();

			// Parsowanie kolumn
			ParseNode selectNode(ParseNode::NodeType::SELECT);
			while (currentIndex < query.size() && query[currentIndex] != 'F') {
				selectNode.addChild(parseColumn());
				skipWhitespace();
			}

			// Parsowanie reszty zapytania
			return selectNode;
		}
		throw std::runtime_error("Invalid SQL syntax");
	}

	ParseNode Parser::parseColumn() {
		size_t start = currentIndex;
		while (currentIndex < query.size() && query[currentIndex] != ',' && query[currentIndex] != ' ') {
			++currentIndex;
		}
		return ParseNode(ParseNode::NodeType::COLUMN, query.substr(start, currentIndex - start));
	}

	void Parser::skipWhitespace() {
		while (currentIndex < query.size() && isspace(query[currentIndex])) {
			++currentIndex;
		}
	}

}