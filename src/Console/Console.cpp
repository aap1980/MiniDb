#include <iostream>
#include "Console.h"
#include "../Parser/Token.h"
#include "../Parser/Lexer.h"
#include "../Parser/AST.h"
#include "../Parser/Parser.h"

namespace MiniDb::Console {

	void Console::run() {
		std::string input;

		std::cout << "MiniDb Console - Enter commands (type 'exit' to quit):\n";

		while (true) {
			std::cout << "> ";
			std::getline(std::cin, input);

			if (input == "exit") {
				break;
			}

			parseCommand(input);
		}
	}

	void Console::parseCommand(const std::string& command) {
		std::istringstream iss(command);
		std::string token;
		iss >> token;

		if (token == "SELECT") {
			handleSelect(iss);
		}
		else if (token == "INSERT") {
			handleInsert(iss);
		}
		else if (token == "UPDATE") {
			handleUpdate(iss);
		}
		else if (token == "DELETE") {
			handleDelete(iss);
		}
		else {
			std::cout << "Error: Unknown command '" << token << "'\n";
		}
	}

	void Console::handleSelect(std::istringstream& iss) {
		std::cout << "Handling SELECT query...\n";

		std::string query;
		query = iss.str();

		try {
			MiniDb::Parser::Lexer lexer(query);
			MiniDb::Parser::Parser parser(lexer);
			MiniDb::Parser::SelectStatement ast = parser.parseSelectStatement();
			std::cout << "Parsed Statement (AST):" << std::endl;
			ast.print();
		}
		catch (const std::exception& e) {
			std::cout << "Error: " << e.what() << "\n";
		}
	}

	void Console::handleInsert(std::istringstream& iss) {
		std::cout << "Handling INSERT query...\n";
		// Tutaj logika obs�ugi zapytania INSERT
	}

	void Console::handleUpdate(std::istringstream& iss) {
		std::cout << "Handling UPDATE query...\n";
		// Tutaj logika obs�ugi zapytania UPDATE
	}

	void Console::handleDelete(std::istringstream& iss) {
		std::cout << "Handling DELETE query...\n";
		// Tutaj logika obs�ugi zapytania DELETE
	}

}
