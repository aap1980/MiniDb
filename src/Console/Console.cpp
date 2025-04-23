#include <iostream>
#include "Console.h"
#include "../Statement/Statement.h"

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
		auto& database = MiniDb::Database::Database::getInstance();
		std::unique_ptr<MiniDb::Statement::Statement> statement = MiniDb::Statement::Statement::fromSQL(command);

		if (statement->returnsResult()) {
			const auto& result = statement->executeResult(database);
			result->print();
		}
		else {
			statement->executeNoResult(database);
		}
	}

}
