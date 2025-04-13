#include <iostream>
#include "Console.h"
#include "../Executors/SelectExecutor.h"
#include "../SqlParser/SQLParser.h"
#include "../SqlParser/SQLParserResult.h"

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
		hsql::SQLParserResult result;
		hsql::SQLParser::parse(command, &result);
	}

}
